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

#include <QLocalSocket>
#include <QSocketNotifier>

#include <sys/errno.h>
#include <sys/socket.h>
#include <errno.h>

#include "connection.h"
#include "fcgi.h"
#include "fdbuilder.h"

QFCgiFdConnectionBuilder::QFCgiFdConnectionBuilder(int fd, QObject *parent)
  : QFCgiConnectionBuilder(parent) {

  this->fd = fd;
  this->notifier = 0;
}

QFCgiFdConnectionBuilder::~QFCgiFdConnectionBuilder() {
}

bool QFCgiFdConnectionBuilder::listen() {
  this->notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
  connect(this->notifier, SIGNAL(activated(int)), this, SLOT(onActivated(int)));
  qDebug("FastCGI application started, listening on %llu", this->notifier->socket());
  return true;
}

bool QFCgiFdConnectionBuilder::isListening() const {
  return (this->notifier != 0);
}

QString QFCgiFdConnectionBuilder::errorString() const {
  return "";
}

void QFCgiFdConnectionBuilder::onActivated(int socket) {
  int so = accept(socket, 0, 0);

  if (so != -1) {
    QFCgi *fcgi = qobject_cast<QFCgi*>(parent());

    QLocalSocket *device = new QLocalSocket(this);
    device->setSocketDescriptor(so, QLocalSocket::ConnectedState, QIODevice::ReadWrite);

    QFCgiConnection *connection = new QFCgiConnection(device, fcgi);
    emit newConnection(connection);
  } else {
    qDebug("accept: %s", strerror(errno));
  }
}
