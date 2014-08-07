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

#ifndef QFCGI_TCP_BUILDER_H
#define QFCGI_TCP_BUILDER_H

#include <QHostAddress>

class QTcpServer;

#include "builder.h"

class QFCgiTcpConnectionBuilder : public QFCgiConnectionBuilder {
  Q_OBJECT

public:
  QFCgiTcpConnectionBuilder(const QHostAddress &address, quint16 port, QObject *parent);
  virtual ~QFCgiTcpConnectionBuilder();

  bool listen();
  bool isListening() const;
  QString errorString() const;

private slots:
  void onNewConnection();

private:
  QTcpServer *server;
  QHostAddress address;
  quint16 port;
};

#endif  /* QFCGI_TCP_BUILDER_H */
