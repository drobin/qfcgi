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
    qDebug() << "record read" << record.getType() << record.getRequestId();
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
