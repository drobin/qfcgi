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

#ifndef QFCGI_REQUEST_H
#define QFCGI_REQUEST_H

#include <QHash>
#include <QObject>
#include <QMetaType>

class QFCgiConnection;
class QFCgiStream;

/**
 * A FastCGI request received from the web-server.
 *
 * Instances of the class are maintained by the library and cannot be created
 * from outside. The QFCgi::newRequest() signal is used to obtain new FastCGI
 * requests.
 *
 * Once a new request is @link QFCgi::newRequest() signaled @endlink, you can
 * obtain the parameters received from the web-server using the #getParams()
 * and #getParam() methods.
 *
 * The #getIn() device buffers the input-data received from the web-server. The
 * usual interface of <code>QIODevice</code> can be used to fetch the
 * input-data. The #getOut() resp. #getErr() devices can be used to send
 * output-data back to the web-server.
 *
 * The final operation should be always an invocation of #endRequest() which
 * terminates the requests and asks to destroy the request-instance.
 */
class QFCgiRequest : public QObject {
  Q_OBJECT

public:
  /**
   * Returns the id of the request.
   *
   * The request-id is part of the FastCGI-specification and is used to identify
   * a request.
   *
   * @return The id the of the request
   */
  int getId() const;

  /**
   * Tests whether the underlaying connection should be closed when the request
   * is @link #endRequest() closed @endlink.
   *
   * This flag is part of the FastCGI-specification and is for information
   * only. The flag is used internally to maintain the underlaying server
   * connection.
   *
   * @return When <code>true</code> is returned, the the underlaying connection
   *         is automatically closed when the request is
   *         @link #endRequest() closed @endlink.
   */
  bool keepConnection() const;

  /**
   * Call the method to signal the end of the request.
   *
   * This method-invocation is always the last action of the request. All
   * streams are closed, and the web-server receives a message, that signals
   * the end of the request. The <code>appStatus</code> is passed back to the
   * web-server, where a value of <code>0</code> usually means success. The
   * current request-object is marked for destruction, which means, that the
   * object will be destroyed when control returns to the Qt event loop.
   *
   * When the method is never invoked, then the request will stay open on the
   * web-server and might result into an error (depending on the web-server).
   *
   * @param appStatus Execution status of the request-operation, where
   *                  <code>0</code> usually means success.
   *
   * @see QObject::deleteLater()
   */
  void endRequest(quint32 appStatus);

  /**
   * Returns a list with the names of all parameters received from the
   * web-server.
   *
   * @return A list with the names of all parameters
   * @see getParam()
   */
  QList<QString> getParams() const;

  /**
   * Returns the value of a parameter received from the web-server.
   *
   * @param name The name of the parameter
   * @return The value of the requested parameter. If the parameter does not
   *         exist, an empty string is returned.
   * @see getParams()
   */
  QString getParam(const QString &name) const;

  /**
   * Returns a stream to receive input-data from the web-server.
   *
   * Arbitrary data are send from the Web server to the application. The
   * returned device is already opened read-only, thus an attempt to write data
   * into the device will lead into an error.
   *
   * @return A stream to receive input-data from the web-server
   */
  QIODevice* getIn() const;

  /**
   * Returns a stream used to send back output-data to the web-server.
   *
   * Arbitrary data can be send back from the application to the web server.
   * The returned device is already opened write-only, this an attempt to read
   * data from the device will lead into an error.
   *
   * @return A stream to write output-data to the web-server.
   * @note The format of the output-data is specified in the
   *       <code>CGI/1.1</code> specification (RFC 3875, section 6).
   */
  QIODevice* getOut() const;

  /**
   * Returns a stream used to send back error-data to the web-server.
   *
   * Arbitrary data can be send back from the application to the web server.
   * The returned device is already opened write-only, this an attempt to read
   * data from the device will lead into an error.
   *
   * @return A stream to write error-data to the web-server.
   */
  QIODevice* getErr() const;

private slots:
  void onOutBytesWritten(qint64 bytes);
  void onErrBytesWritten(qint64 bytes);

private:
  friend class QFCgiConnection;

  QFCgiRequest(int id, bool keepConn, QFCgiConnection *parent);
  virtual ~QFCgiRequest() {}

  void consumeParamsBuffer(const QByteArray &data);
  qint32 readNameValuePair(QString &name, QString &value);
  qint32 readLengthField(int pos, quint32 *length);
  qint32 readValueField(int pos, quint32 length, QString &value);

  int id;
  bool keepConn;
  QByteArray paramsBuffer;
  QFCgiStream *in;
  QFCgiStream *out;
  QFCgiStream *err;
  QHash<QString, QString> params;
};

Q_DECLARE_METATYPE(QFCgiRequest*);

#endif  /* QFCGI_REQUEST_H */
