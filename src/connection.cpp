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

#include <QTcpSocket>

#include "connection.h"
#include "qfcgi.h"
#include "record.h"
#include "request.h"

#define FCGI_BEGIN_REQUEST       1
#define FCGI_ABORT_REQUEST       2
#define FCGI_END_REQUEST         3
#define FCGI_PARAMS              4
#define FCGI_STDIN               5
#define FCGI_STDOUT              6
#define FCGI_STDERR              7
#define FCGI_DATA                8
#define FCGI_GET_VALUES          9
#define FCGI_GET_VALUES_RESULT  10
#define FCGI_UNKNOWN_TYPE       11
#define FCGI_MAXTYPE (FCGI_UNKNOWN_TYPE)

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

QFCgiConnection::QFCgiConnection(QTcpSocket *so, QFCgi *parent) : QObject(parent) {
  this->so = so;

  connect(this->so, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

QFCgiConnection::~QFCgiConnection() {
  delete so;
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
}

void QFCgiConnection::fillBuffer() {
  qint64 avail = this->so->bytesAvailable();
  char buf[avail];

  qint64 nread = this->so->read(buf, avail);

  if (nread >= 0) {
    qDebug() << nread << "bytes read from socket";
    this->buf.append(buf, nread);
  } else {
    qCritical() << this->so->errorString();
    deleteLater();
  }
}

void QFCgiConnection::handleManagementRecord(QFCgiRecord &record) {
  qDebug() << "management record read" << record.getType() << record.getRequestId();
}

void QFCgiConnection::handleApplicationRecord(QFCgiRecord &record) {
  switch (record.getType()) {
    case FCGI_BEGIN_REQUEST: handleFCGI_BEGIN_REQUEST(record); break;
    case FCGI_PARAMS: handleFCGI_PARAMS(record); break;
  }
}

void QFCgiConnection::handleFCGI_BEGIN_REQUEST(QFCgiRecord &record) {
  QByteArray &ba = record.getContent();
  quint16 role = ((ba[0] & 0xFF) << 8) | (ba[1] & 0xFF);
  quint8 flags = ba[2];
  bool keep_conn = ((flags & FCGI_KEEP_CONN) > 0);

  if (role != FCGI_RESPONDER) {
    qCritical() << "new FastCGI request (unsupported role) [ id:" << record.getRequestId() << ", role:" << role << ", keep_conn:" << keep_conn << "]";
    QFCgiRecord response = QFCgiRecord::createEndRequest(record.getRequestId(), 0, QFCgiRecord::FCGI_UNKNOWN_ROLE);
    response.write(this->so);

    return;
  }

  QFCgiRequest *request = new QFCgiRequest(record.getRequestId(), keep_conn, this);
  this->requests.insert(request->getId(), request);
  qDebug() << "new FastCGI request [ id:" << request->getId() << ", role:" << role << ", keep_conn:" << keep_conn << "]";
}

void QFCgiConnection::handleFCGI_PARAMS(QFCgiRecord &record) {
  QFCgiRequest *request = this->requests.value(record.getRequestId(), 0);

  if (request == 0) {
    qCritical() << "could not find request with id" << record.getRequestId();
    deleteLater();
    return;
  }

  qDebug() << "[" << request->getId() << "]: FCGI_PARAMS";
}
