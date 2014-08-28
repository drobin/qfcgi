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

#ifndef QFCGI_FCGI_H
#define QFCGI_FCGI_H

#include <QObject>

class QFCgiConnection;
class QFCgiConnectionBuilder;
class QFCgiRequest;
class QHostAddress;

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
   * File descriptor where the FastCGI application server can accept
   * connections.
   */
  enum FileDescriptor {
    /**
     *  Accept connections on <code>stdin</code>.
     */
    FCGI_LISTENSOCK_FILENO = 0
  };

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
   * Configures the FastCGI application server for listening on the given
   * UNIX domain socket.
   *
   * @param path The path to the UNIX domain socket
   */
  void configureListen(const QString &path);

  /**
   * Configures the FastCGI application server for listening on the given
   * file descriptor.
   *
   * According the the FastCGI specification the application server can
   * accept incoming connections on #FCGI_LISTENSOCK_FILENO which equals to
   * <code>stdin</code>. This mode is used, when the web server forks the
   * application server process, where the web server binds the socket used for
   * communication with the application server and maps the socket to
   * <code>stdin</code> of the application server process. Use this mode, when
   * the FastCGI applications server process is under control of the web
   * server.
   *
   * @param fd The file descriptor where the application server accepts new
   *           connections.
   */
  void configureListen(enum FileDescriptor fd);

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
   *
   * You need to configure the FastCGI application server by calling one of the
   * <code>configureListen</code> methods.
   *
   * @see configureListen(const QHostAddress &address, quint16 port)
   * @see configureListen(const QString &path)
   * @see configureListen(enum FileDescriptor fd)
   */
  void start();

private slots:
  void onNewConnection(QFCgiConnection *connection);

private:
  friend class QFCgiConnection;

  void updateBuilder(QFCgiConnectionBuilder *builder);

  QFCgiConnectionBuilder *builder;
};

#endif  /* QFCGI_FCGI_H */
