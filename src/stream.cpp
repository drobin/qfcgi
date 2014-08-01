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

#include "stream.h"

QFCgiStream::QFCgiStream(QObject *parent) : QIODevice(parent) {
}

QFCgiStream::~QFCgiStream() {
}

void QFCgiStream::append(const QByteArray &ba) {
  this->array.append(ba);
  emit readyRead();
}

qint64 QFCgiStream::readData(char *data, qint64 maxSize) {
  if (this->array.isEmpty()) {
    return -1;
  }

  qint64 nbytes = qMin(this->array.size(), (int)maxSize);
  memcpy(data, this->array.data(), nbytes);

  return nbytes;
}

qint64 QFCgiStream::writeData(const char *data, qint64 maxSize) {
  this->array.append(data, maxSize);
  return maxSize;
}
