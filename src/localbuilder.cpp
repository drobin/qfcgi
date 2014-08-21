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

#include <QLocalServer>
#include <QLocalSocket>

#include "connection.h"
#include "fcgi.h"
#include "localbuilder.h"

QFCgiLocalConnectionBuilder::QFCgiLocalConnectionBuilder(const QString &path, QObject *parent)
  : QFCgiConnectionBuilder(parent) {

  this->server = new QLocalServer(this);
  this->path = path;
}

QFCgiLocalConnectionBuilder::~QFCgiLocalConnectionBuilder() {
}

bool QFCgiLocalConnectionBuilder::listen() {
  if (this->server->listen(this->path)) {
    connect(this->server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    qDebug("FastCGI application started, listening on %s",
      qPrintable(this->server->fullServerName()));
    return true;
  } else {
    return false;
  }
}

bool QFCgiLocalConnectionBuilder::isListening() const {
  return this->server->isListening();
}

QString QFCgiLocalConnectionBuilder::errorString() const {
  return this->server->errorString();
}

void QFCgiLocalConnectionBuilder::onNewConnection() {
  QFCgi *fcgi = qobject_cast<QFCgi*>(parent());
  QLocalSocket *so = this->server->nextPendingConnection();
  QFCgiConnection *connection = new QFCgiConnection(so, fcgi);
  emit newConnection(connection);
}
