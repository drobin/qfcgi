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
#include <QDebug>

#include "connection.h"
#include "request.h"
#include "stream.h"

#define q2Debug() qDebug() << "[" << this->id << "]"

QFCgiRequest::QFCgiRequest(int id, bool keepConn, QFCgiConnection *parent) : QObject(parent) {
  this->id = id;
  this->keepConn = keepConn;
  this->paramsBuffer = new QBuffer(this);
  this->in = new QFCgiStream(this);

  this->paramsBuffer->open(QBuffer::ReadWrite);
  this->in->open(QIODevice::ReadOnly);

  connect(this->paramsBuffer, SIGNAL(readyRead()), this, SLOT(onParamsReadyRead()));
}

int QFCgiRequest::getId() const {
  return this->id;
}

bool QFCgiRequest::keepConnection() const {
  return this->keepConn;
}

QList<QString> QFCgiRequest::getParams() const {
  return this->params.keys();
}

QString QFCgiRequest::getParam(const QString &name) const {
  return this->params.value(name);
}

QIODevice* QFCgiRequest::getIn() {
  return this->in;
}

void QFCgiRequest::onParamsReadyRead() {
  qint32 nread;
  QString name, value;

  while ((nread = readNameValuePair(name, value)) > 0) {
    q2Debug() << "param(" << name << ") =" << value;
    this->paramsBuffer->buffer().remove(0, nread);
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
  QByteArray ba = this->paramsBuffer->buffer().mid(pos);

  if (ba.isEmpty()) {
    return 0;
  }

  *length = ba[0] & 0xFF;

  if (*length >> 7 == 0) {
    return 1;
  }

  if (ba.size() < 4) {
    return 0;
  }

  for (int i = 1; i < 4; i++) {
    *length <<= 8;
    *length |= (ba[i] & 0xFF);
  }

  return 4;
}

qint32 QFCgiRequest::readValueField(int pos, quint32 length, QString &value) {
  QByteArray ba = this->paramsBuffer->buffer().mid(pos);

  if (ba.size() >= (int)length) {
    value = ba.left(length);
    return length;
  } else {
    return 0;
  }
}
