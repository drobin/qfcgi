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

#include <QHostAddress>
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
 * Before the application server can be started, you need to configure it with
 * the #configureListen() method. By default the server is listening on
 * <code>anyhost (0.0.0.0)</code> and port <code>9000</code>.
 *
 * For reach request received from the web server the #newRequest() signal is
 * emitted.
 */
class QFCgi : public QObject {
  Q_OBJECT

public:
  /**
   * Creates a new instance of the class.
   */
  QFCgi(QObject *parent = 0);
  virtual ~QFCgi();

  /**
   * Configures the FastCGI application server for listening on the given
   * address and port.
   *
   * After a #start() invocation the application server accepts TCP connections
   * on the adress/port combination.
   *
   * @param address IP address
   * @param port Port number
   */
  void configureListen(const QHostAddress &address, quint16 port);

  /**
   * Tests whether the #start() operation was successful.
   *
   * If succeeded, the FastCGI application server is waiting for incoming
   * connections. If <code>false</code> is returned, then either #start() was
   * not called or the application server listen information are not configured
   * properly. Check one of the <code>configureListen</code> methods. You can
   * call #errorString() to receive a meaningful error message.
   *
   * @return If <code>true</code> is returned, the application server is ready
   *         and waiting for incoming connections.
   */
  bool isStarted() const;

  /**
   * In case of an startup error, this method returns a (more) meaningful error
   * message.
   *
   * @return Error message that describes the error in detail.
   */
  QString errorString() const;

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
   * You need to call this method to setup the server. To test whether the
   * start-operation was successful, call #isStarted().
   */
  void start();

private slots:
  void onNewConnection();

private:
  friend class QFCgiConnection;

  QTcpServer *server;
  QHostAddress listenAddress;
  quint16 listenPort;
};

#endif  /* QFCGI_H */
