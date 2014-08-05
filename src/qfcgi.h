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

#ifndef QFCGI_H
#define QFCGI_H

#include <QObject>

class QFCgiRequest;
class QTcpServer;

/**
 * FastCGI support for Qt.
 *
 * This class is the entry point for the library. The library uses the Qt event
 * mechanismn to dispatch its data. To start the FastCGI application server
 * call the #start() method.
 *
 * For reach request received from the web server the #newRequest() signal is emitted.
 */
class QFCgi : public QObject {
  Q_OBJECT

public:
  /**
   * Creates a new instance of the class.
   */
  QFCgi(QObject *parent = 0);
  virtual ~QFCgi();

signals:
  /**
   * This signal is emitted when a new request was received from the web server.
   *
   * The library takes over the ownership of the request-object, thus don't
   * destroy the object by yourself.
   *
   * @param request The new request
   */
  void newRequest(QFCgiRequest *request);

public slots:
  /**
   * Starts the FastCGI application server.
   *
   * You need to call this method to setup the server.
   */
  void start();

private slots:
  void onNewConnection();

private:
  friend class QFCgiConnection;

  QTcpServer *server;
};

#endif  /* QFCGI_H */
