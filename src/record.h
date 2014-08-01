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

  QFCgiRecord();
  QFCgiRecord(quint8 type, quint16 requestId);
  QFCgiRecord(const QFCgiRecord &other);

  QFCgiRecord& operator = (const QFCgiRecord &other);

  enum Version getVersion() const;
  quint8 getType() const;
  void setType(quint8 type);
  quint16 getRequestId() const;
  void setRequestId(quint16 requestId);
  QByteArray& getContent();

  qint32 read(const QByteArray &ba);
  qint32 write(QIODevice *device);

private:
  bool setVersion(quint8 version);

  qint32 readHeader(const QByteArray &ba, quint16 *contentLength, quint8 *paddingLength);
  qint32 writeHeader(QIODevice *device, quint8 *paddingLength);

  enum Version version;
  quint8 type;
  quint16 requestId;
  QByteArray content;
};

#endif  /* QFCGI_RECORD_H */
