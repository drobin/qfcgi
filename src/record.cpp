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

#include "record.h"

QFCgiRecord::QFCgiRecord() {
  QFCgiRecord(0, 0);
}

QFCgiRecord::QFCgiRecord(quint8 type, quint16 requestId) {
  this->version = QFCgiRecord::V1;
  this->type = type;
  this->requestId = requestId;
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

enum QFCgiRecord::Version QFCgiRecord::getVersion() const {
  return this->version;
}

quint8 QFCgiRecord::getType() const {
  return this->type;
}

void QFCgiRecord::setType(quint8 type) {
  this->type = type;
}

quint16 QFCgiRecord::getRequestId() const {
  return this->requestId;
}

void QFCgiRecord::setRequestId(quint16 requestId) {
  this->requestId = requestId;
}

QByteArray& QFCgiRecord::getContent() {
  return this->content;
}
