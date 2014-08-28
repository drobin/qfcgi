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
#include "fdbuilder.h"
#include "localbuilder.h"
#include "tcpbuilder.h"

QFCgi::QFCgi(QObject *parent) : QObject(parent) {
  this->builder = new QFCgiTcpConnectionBuilder(QHostAddress::Any, 9000, this);
}

QFCgi::~QFCgi() {

}

void QFCgi::configureListen(const QHostAddress &address, quint16 port) {
  updateBuilder(new QFCgiTcpConnectionBuilder(address, port, this));
}

void QFCgi::configureListen(const QString &path) {
  updateBuilder(new QFCgiLocalConnectionBuilder(path, this));
}

void QFCgi::configureListen(enum FileDescriptor fd) {
  updateBuilder(new QFCgiFdConnectionBuilder(fd, this));
}

bool QFCgi::isStarted() const {
  return (this->builder != 0) && this->builder->isListening();
}

QString QFCgi::errorString() const {
  if (this->builder != 0) {
    return this->builder->errorString();
  } else {
    return "not started";
  }
}

void QFCgi::start() {
  if (this->builder->listen()) {
    connect(this->builder, SIGNAL(newConnection(QFCgiConnection*)),
            this, SLOT(onNewConnection(QFCgiConnection*)));
  } else {
    qDebug("failed to start FastCGI application: %s", qPrintable(this->builder->errorString()));
  }
}

void QFCgi::onNewConnection(QFCgiConnection *connection) {
  qDebug("[%d] FastCGI connection accepted", connection->getId());
}

void QFCgi::updateBuilder(QFCgiConnectionBuilder *builder) {
  delete this->builder;
  this->builder = builder;
}
