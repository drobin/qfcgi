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

#ifndef QFCGI_RECORD_H
#define QFCGI_RECORD_H

#include <QByteArray>
#include <QtGlobal>

class QIODevice;

class QFCgiRecord {
public:
  enum Version {
    V1 = 1
  };

  enum Type {
    FCGI_BEGIN_REQUEST = 1,
    FCGI_ABORT_REQUEST = 2,
    FCGI_END_REQUEST = 3,
    FCGI_PARAMS = 4,
    FCGI_STDIN = 5,
    FCGI_STDOUT = 6,
    FCGI_STDERR = 7,
    FCGI_DATA = 8,
    FCGI_GET_VALUES = 9,
    FCGI_GET_VALUES_RESULT = 10,
    FCGI_UNKNOWN_TYPE = 11
  };

  enum ProtocolStatus {
    FCGI_REQUEST_COMPLETE = 0,
    FCGI_CANT_MPX_CONN = 1,
    FCGI_OVERLOADED = 2,
    FCGI_UNKNOWN_ROLE = 3
  };

  QFCgiRecord();
  QFCgiRecord(const QFCgiRecord &other);

  static QFCgiRecord createEndRequest(quint32 requestId, quint32 appStatus, enum ProtocolStatus protocolStatus);
  static QFCgiRecord createOutStream(quint32 requestId, const QByteArray &data);
  static QFCgiRecord createErrStream(quint32 requestId, const QByteArray &data);
  static QFCgiRecord createDataStream(quint32 requestId, const QByteArray &data);

  QFCgiRecord& operator = (const QFCgiRecord &other);

  enum Version getVersion() const;
  enum Type getType() const;
  void setType(Type type);
  quint16 getRequestId() const;
  void setRequestId(quint16 requestId);
  const QByteArray& getContent() const;

  qint32 read(const QByteArray &ba);
  qint32 write(QIODevice *device) const;

private:
  bool setVersion(quint8 version);
  bool setType(quint8 type);

  qint32 readHeader(const QByteArray &ba, quint16 *contentLength, quint8 *paddingLength);
  qint32 writeHeader(QIODevice *device, quint8 *paddingLength) const;

  enum Version version;
  enum Type type;
  quint16 requestId;
  QByteArray content;
};

#endif  /* QFCGI_RECORD_H */
