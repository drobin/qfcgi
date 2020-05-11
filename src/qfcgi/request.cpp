/**
 * This file is part of QFCgi.
 *
 * QFCgi is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * QFCgi is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with QFCgi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QBuffer>
#include <QtGlobal>

#include "connection.h"
#include "record.h"
#include "request.h"
#include "stream.h"

#define q2Debug(format, args...) qDebug("[%d] " format, this->id, ##args)

QFCgiRequest::QFCgiRequest(int id, bool keepConn, QFCgiConnection *parent) : QObject(parent) {
  this->id = id;
  this->keepConn = keepConn;
  this->in = new QFCgiStream(this);
  this->out = new QFCgiStream(this);
  this->err = new QFCgiStream(this);

  this->in->open(QIODevice::ReadOnly);
  this->out->open(QIODevice::WriteOnly);
  this->err->open(QIODevice::WriteOnly);

  connect(this->out, SIGNAL(bytesWritten(qint64)), this, SLOT(onOutBytesWritten(qint64)));
  connect(this->err, SIGNAL(bytesWritten(qint64)), this, SLOT(onErrBytesWritten(qint64)));
}

int QFCgiRequest::getId() const {
  return this->id;
}

bool QFCgiRequest::keepConnection() const {
  return this->keepConn;
}

void QFCgiRequest::endRequest(quint32 appStatus) {
  QFCgiConnection *connection = qobject_cast<QFCgiConnection*>(parent());

  connection->send(QFCgiRecord::createOutStream(this->id, QByteArray()));
  connection->send(QFCgiRecord::createErrStream(this->id, QByteArray()));
  connection->send(QFCgiRecord::createEndRequest(this->id, appStatus, QFCgiRecord::FCGI_REQUEST_COMPLETE));

  if (!keepConnection()) {
    q2Debug("endRequest - about to close connection");
    connection->closeConnection();
  }
}

QList<QString> QFCgiRequest::getParams() const {
  return this->params.keys();
}

QString QFCgiRequest::getParam(const QString &name) const {
  return this->params.value(name);
}

QIODevice* QFCgiRequest::getIn() const {
  return this->in;
}

QIODevice* QFCgiRequest::getOut() const {
  return this->out;
}

QIODevice* QFCgiRequest::getErr() const {
  return this->err;
}

void QFCgiRequest::onOutBytesWritten(qint64 bytes) {
  QByteArray &ba = this->out->getBuffer();
  int nbytes = qMin(65535, ba.size());

  QFCgiConnection *connection = qobject_cast<QFCgiConnection*>(parent());
  QFCgiRecord record = QFCgiRecord::createOutStream(this->id, ba.left(nbytes));

  ba.remove(0, nbytes);
  connection->send(record);
}

void QFCgiRequest::onErrBytesWritten(qint64 bytes) {
  QByteArray &ba = this->err->getBuffer();
  int nbytes = qMin(65535, ba.size());

  QFCgiConnection *connection = qobject_cast<QFCgiConnection*>(parent());
  QFCgiRecord record = QFCgiRecord::createErrStream(this->id, ba.left(nbytes));

  ba.remove(0, nbytes);
  connection->send(record);
}

void QFCgiRequest::consumeParamsBuffer(const QByteArray &data) {
  qint32 nread;
  QString name, value;

  this->paramsBuffer.append(data);

  while ((nread = readNameValuePair(name, value)) > 0) {
    q2Debug("param(%s): %s", qPrintable(name), qPrintable(value));
    this->paramsBuffer.remove(0, nread);
    this->params.insert(name, value);
  }
}

qint32 QFCgiRequest::readNameValuePair(QString &name, QString &value) {
  quint32 nameLength, valueLength;
  qint32 nnl, nvl, nn, nv;

  if ((nnl = readLengthField(0, &nameLength)) <= 0) {
    return nnl;
  }

  if ((nvl = readLengthField(nnl, &valueLength)) <= 0) {
    return nvl;
  }

  if ((nn = readValueField(nnl + nvl, nameLength, name)) <= 0) {
    return nn;
  }

  if ((nv = readValueField(nnl + nvl + nn, valueLength, value)) <= 0) {
    return nv;
  }

  return nnl + nvl + nn + nv;
}

qint32 QFCgiRequest::readLengthField(int pos, quint32 *length) {
  QByteArray ba = this->paramsBuffer.mid(pos);

  if (ba.isEmpty()) {
    return 0;
  }

  if ((ba[0] & 0x80) == 0) {
    *length = ba[0] & 0xFF;
    return 1;
  }

  if (ba.size() < 4) {
    return 0;
  }

  *length = ((ba[0] & 0x7F) << 24) |
            ((ba[1] & 0xFF) << 16) |
            ((ba[2] & 0xFF) << 8) |
            (ba[3] & 0xFF);

  return 4;
}

qint32 QFCgiRequest::readValueField(int pos, quint32 length, QString &value) {
  QByteArray ba = this->paramsBuffer.mid(pos);

  if (ba.size() >= (int)length) {
    value = ba.left(length);
    return length;
  } else {
    return 0;
  }
}
