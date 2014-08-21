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

#include <QTcpServer>
#include <QTcpSocket>

#include "connection.h"
#include "fcgi.h"
#include "tcpbuilder.h"

QFCgiTcpConnectionBuilder::QFCgiTcpConnectionBuilder(const QHostAddress &address, quint16 port, QObject *parent)
  : QFCgiConnectionBuilder(parent) {

  this->server = new QTcpServer(this);
  this->address = address;
  this->port = port;
}

QFCgiTcpConnectionBuilder::~QFCgiTcpConnectionBuilder() {
}

bool QFCgiTcpConnectionBuilder::listen() {
  if (this->server->listen(this->address, this->port)) {
    connect(this->server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    qDebug("FastCGI application started, listening on %s/%d",
      qPrintable(this->server->serverAddress().toString()),
      this->server->serverPort());
    return true;
  } else {
    return false;
  }
}

bool QFCgiTcpConnectionBuilder::isListening() const {
  return this->server->isListening();
}

QString QFCgiTcpConnectionBuilder::errorString() const {
  return this->server->errorString();
}

void QFCgiTcpConnectionBuilder::onNewConnection() {
  QFCgi *fcgi = qobject_cast<QFCgi*>(parent());
  QTcpSocket *so = this->server->nextPendingConnection();
  QFCgiConnection *connection = new QFCgiConnection(so, fcgi);
  emit newConnection(connection);
}
