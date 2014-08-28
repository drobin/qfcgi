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

#include "connection.h"
#include "fcgi.h"
#include "record.h"
#include "request.h"
#include "stream.h"

#define q1Debug(format, args...) qDebug("[%d] " format, this->id, ##args)
#define q2Debug(record, format, args...) qDebug("[%d,%d] " format, this->id, record.getRequestId(), ##args)

/*
 * Values for role component of FCGI_BeginRequestBody
 */
#define FCGI_RESPONDER  1
#define FCGI_AUTHORIZER 2
#define FCGI_FILTER     3

/*
 * Mask for flags component of FCGI_BeginRequestBody
 */
#define FCGI_KEEP_CONN  1

static int nextConnectionId = 0;

QFCgiConnection::QFCgiConnection(QIODevice *device, QFCgi *parent) : QObject(parent) {
  this->id = ++nextConnectionId;
  this->device = device;
  this->device->setParent(this); /* Take over ownership of the device.
                                    You it is safe to destroy the object here. */

  connect(this->device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
  connect(this->device, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

QFCgiConnection::~QFCgiConnection() {
  delete this->device;
}

int QFCgiConnection::getId() const {
  return this->id;
}

void QFCgiConnection::send(const QFCgiRecord &record) {
  q2Debug(record, "sending record [type: %d, content-length: %d]", record.getType(), record.getContent().size());
  record.write(this->device);
}

void QFCgiConnection::closeConnection() {
  this->device->close();
}

void QFCgiConnection::onReadyRead() {
  fillBuffer();

  QFCgiRecord record;
  qint32 nconsumed;

  while ((nconsumed = record.read(this->buf)) > 0) {
    this->buf.remove(0, nconsumed);

    switch (record.getRequestId()) {
      case 0:  handleManagementRecord(record); break;
      default: handleApplicationRecord(record); break;
    }
  }

  if (nconsumed < 0) {
    q1Debug("failed to read record");
    deleteLater();
  }
}

void QFCgiConnection::onDisconnected() {
  q1Debug("FastCGI connection closed");
  deleteLater();
}

void QFCgiConnection::fillBuffer() {
  qint64 avail = this->device->bytesAvailable();
  char buf[avail];

  qint64 nread = this->device->read(buf, avail);

  if (nread >= 0) {
    q1Debug("%lli bytes read from socket", nread);
    this->buf.append(buf, nread);
  } else {
    q1Debug("%s", qPrintable(this->device->errorString()));
    deleteLater();
  }
}

void QFCgiConnection::handleManagementRecord(QFCgiRecord &record) {
  q2Debug(record, "management record read");
}

void QFCgiConnection::handleApplicationRecord(QFCgiRecord &record) {
  QFCgiRequest *request = this->requests.value(record.getRequestId(), 0);

  if (request == 0 && record.getType() != QFCgiRecord::FCGI_BEGIN_REQUEST) {
    q2Debug(record, "no such request");
    deleteLater();
    return;
  }

  switch (record.getType()) {
    case QFCgiRecord::FCGI_BEGIN_REQUEST: handleFCGI_BEGIN_REQUEST(record); break;
    case QFCgiRecord::FCGI_PARAMS: handleFCGI_PARAMS(request, record); break;
    case QFCgiRecord::FCGI_STDIN: handleFCGI_STDIN(request, record); break;
    default: q2Debug(record, "invalid record of type %d", record.getType());
  }
}

void QFCgiConnection::handleFCGI_BEGIN_REQUEST(QFCgiRecord &record) {
  const QByteArray &ba = record.getContent();
  quint16 role = ((ba[0] & 0xFF) << 8) | (ba[1] & 0xFF);
  quint8 flags = ba[2];
  bool keep_conn = ((flags & FCGI_KEEP_CONN) > 0);

  if (role == FCGI_RESPONDER) {
    if (this->requests.contains(record.getRequestId())) {
      q2Debug(record, "new FastCGI request (invalid request-id) [role: %d, keep_conn: %d]", role, keep_conn);

      QFCgiRecord response = QFCgiRecord::createEndRequest(record.getRequestId(), 0, QFCgiRecord::FCGI_OVERLOADED);
      send(response);

      closeConnection();
    } else {
      QFCgiRequest *request = new QFCgiRequest(record.getRequestId(), keep_conn, this);
      this->requests.insert(request->getId(), request);
      q2Debug(record, "new FastCGI request [role: %d, keep_conn: %d]", role, keep_conn);
    }
  } else {
    bool valid = validateRole(role);

    q2Debug(record, "new FastCGI request (%s role) [role: %d, keep_conn: %d]",
      (valid ? "unsupported" : "invalid"), role, keep_conn);

    QFCgiRecord response = QFCgiRecord::createEndRequest(record.getRequestId(), 0, QFCgiRecord::FCGI_UNKNOWN_ROLE);
    send(response);

    if (!valid) {
      // an invalid role will always close the connection
      closeConnection();
    }
  }
}

void QFCgiConnection::handleFCGI_PARAMS(QFCgiRequest *request, QFCgiRecord &record) {
  const QByteArray &ba = record.getContent();

  if (!ba.isEmpty()) {
    q2Debug(record, "FCGI_PARAMS");
    request->consumeParamsBuffer(ba);
  } else {
    q2Debug(record, "FCGI_PARAMS (end of stream)");
    QFCgi *fcgi = qobject_cast<QFCgi*>(parent());
    emit fcgi->newRequest(request);
  }
}

void QFCgiConnection::handleFCGI_STDIN(QFCgiRequest *request, QFCgiRecord &record) {
  const QByteArray &ba = record.getContent();

  if (!ba.isEmpty()) {
    q2Debug(record, "FCGI_STDIN");
    request->in->append(ba);
  } else {
    q2Debug(record, "FCGI_STDIN (end of stream)");
    request->in->setEof();
  }
}

bool QFCgiConnection::validateRole(quint16 role) const {
  switch (role) {
    case FCGI_RESPONDER:
    case FCGI_AUTHORIZER:
    case FCGI_FILTER:
      return true;
    default:
      return false;
  }
}
