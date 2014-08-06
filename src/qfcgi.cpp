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
#include "qfcgi.h"

QFCgi::QFCgi(QObject *parent) : QObject(parent) {
  this->server = 0;
  this->listenAddress = QHostAddress::Any;
  this->listenPort = 9000;
}

QFCgi::~QFCgi() {

}

void QFCgi::configureListen(const QHostAddress &address, quint16 port) {
  this->listenAddress = address;
  this->listenPort = port;
}

bool QFCgi::isStarted() const {
  return (this->server != 0) && this->server->isListening();
}

QString QFCgi::errorString() const {
  if (this->server != 0) {
    return this->server->errorString();
  } else {
    return "not started";
  }
}

void QFCgi::start() {
  this->server = new QTcpServer(this);

  if (this->server->listen(this->listenAddress, this->listenPort)) {
    connect(this->server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    qDebug() << "FastCGI application started, listening on"
             << this->server->serverAddress().toString()
             << "/"
             << this->server->serverPort();
  } else {
    qCritical() << "failed to start FastCGI application:" << this->server->errorString();
  }
}

void QFCgi::onNewConnection() {
  QTcpSocket *so = this->server->nextPendingConnection();
  QFCgiConnection *connection = new QFCgiConnection(so, this);
  qDebug() << "[" << connection->getId() << "]" << "FastCGI connection accepted";
}
