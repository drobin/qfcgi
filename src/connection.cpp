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

void QFCgiConnection::handleManagementRecord(const QFCgiRecord &record) {
  qDebug() << "management record read" << record.getType() << record.getRequestId();
}

void QFCgiConnection::handleApplicationRecord(const QFCgiRecord &record) {
  switch (record.getType()) {
    case FCGI_BEGIN_REQUEST: handleFCGI_BEGIN_REQUEST(record); break;
  }
}

void QFCgiConnection::handleFCGI_BEGIN_REQUEST(const QFCgiRecord &record) {
  QFCgiRequest *request = new QFCgiRequest(record.getRequestId(), this);
  this->records.insert(request->getId(), request);
  qDebug() << "new FastCGI request:" << request->getId();
}
