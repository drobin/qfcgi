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

#include <QIODevice>

#include "record.h"

/*
 * Number of bytes in a FCGI_Header. Future versions of the protocol
 * will not reduce this number.
 */
#define FCGI_HEADER_LEN 8

/*
 * Value for version component of FCGI_Header
 */
#define FCGI_VERSION_1 1

QFCgiRecord::QFCgiRecord() {
  this->version = QFCgiRecord::V1;
  this->type = FCGI_UNKNOWN_TYPE;
  this->requestId = 0;
}

QFCgiRecord::QFCgiRecord(const QFCgiRecord &other) {
  this->version = other.version;
  this->type = other.type;
  this->requestId = other.requestId;
  this->content = other.content;
}

QFCgiRecord& QFCgiRecord::operator = (const QFCgiRecord &other) {
  this->version = other.version;
  this->type = other.type;
  this->requestId = other.requestId;
  this->content = other.content;

  return *this;
}

QFCgiRecord QFCgiRecord::createEndRequest(quint32 requestId, quint32 appStatus, enum ProtocolStatus protocolStatus) {
  const char reserved[] = { 0, 0, 0 };

  QFCgiRecord record;
  record.type = FCGI_END_REQUEST;
  record.requestId = requestId;

  record.content
    .append((appStatus >> 24) & 0xFF)
    .append((appStatus >> 16) & 0xFF)
    .append((appStatus >> 8) & 0xFF)
    .append(appStatus & 0xFF)
    .append(protocolStatus)
    .append(reserved, sizeof(reserved));

  return record;
}

QFCgiRecord QFCgiRecord::createOutStream(quint32 requestId, const QByteArray &data) {
  QFCgiRecord record;

  record.type = FCGI_STDOUT;
  record.requestId = requestId;
  record.content = data;

  return record;
}

QFCgiRecord QFCgiRecord::createErrStream(quint32 requestId, const QByteArray &data) {
  QFCgiRecord record;

  record.type = FCGI_STDERR;
  record.requestId = requestId;
  record.content = data;

  return record;
}

QFCgiRecord QFCgiRecord::createDataStream(quint32 requestId, const QByteArray &data) {
  QFCgiRecord record;

  record.type = FCGI_DATA;
  record.requestId = requestId;
  record.content = data;

  return record;
}

enum QFCgiRecord::Version QFCgiRecord::getVersion() const {
  return this->version;
}

bool QFCgiRecord::setVersion(quint8 version) {
  if (version == FCGI_VERSION_1) {
    this->version = QFCgiRecord::V1;
    return true;
  } else {
    return false;
  }
}

enum QFCgiRecord::Type QFCgiRecord::getType() const {
  return this->type;
}

void QFCgiRecord::setType(QFCgiRecord::Type type) {
  this->type = type;
}

bool QFCgiRecord::setType(quint8 type) {
  if (type > 0 && type <= FCGI_UNKNOWN_TYPE) {
    this->type = (enum Type)type;
    return true;
  } else {
    return false;
  }
}

quint16 QFCgiRecord::getRequestId() const {
  return this->requestId;
}

void QFCgiRecord::setRequestId(quint16 requestId) {
  this->requestId = requestId;
}

const QByteArray& QFCgiRecord::getContent() const {
  return this->content;
}

qint32 QFCgiRecord::read(const QByteArray &ba) {
  quint16 contentLength;
  quint8 paddingLength;

  qint32 nread = readHeader(ba, &contentLength, &paddingLength);

  if (nread <= 0) {
    return nread;
  }

  if (ba.size() < nread + contentLength + paddingLength) {
    return 0;
  }

  this->content = ba.mid(nread, contentLength);

  // don't read padding but skip it
  return nread + contentLength + paddingLength;
}

qint32 QFCgiRecord::write(QIODevice *device) const {
  quint8 paddingLength;
  qint32 nwritten;

  nwritten = writeHeader(device, &paddingLength);
  device->write(this->content);

  QByteArray padding(paddingLength, 0);
  device->write(padding);

  return nwritten + this->content.size() + paddingLength;
}

qint32 QFCgiRecord::readHeader(const QByteArray &ba, quint16 *contentLength, quint8 *paddingLength) {
  if (ba.size() < FCGI_HEADER_LEN) {
    // Not enough data available
    return 0;
  }

  if (!setVersion(ba[0] & 0xFF)) {
    return -1;
  }

  if (!setType(ba[1] & 0xFF)) {
    return -1;
  }

  this->requestId = ((ba[2] & 0xFF) << 8) | (ba[3] & 0xFF);

  *contentLength = ((ba[4] & 0xFF) << 8) | (ba[5] & 0xFF);
  *paddingLength = ba[6] & 0xFF;

  // ba[7] -> reserved-flag

  return FCGI_HEADER_LEN;
}

qint32 QFCgiRecord::writeHeader(QIODevice *device, quint8 *paddingLength) const {
  int contentLength = this->content.size();
  int reserved = 0;

  int mod = contentLength % FCGI_HEADER_LEN;
  *paddingLength = (mod > 0) ? FCGI_HEADER_LEN - mod : 0;

  QByteArray header;
  header.append(FCGI_VERSION_1)
    .append(this->type)
    .append((this->requestId >> 8) & 0xFF).append(this->requestId & 0xFF)
    .append((contentLength >> 8) & 0xFF).append(contentLength & 0xFF)
    .append(*paddingLength & 0xFF)
    .append(reserved);
  device->write(header);

  return FCGI_HEADER_LEN;
}
