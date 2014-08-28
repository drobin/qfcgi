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

#define is_readable() ((openMode() & QIODevice::ReadOnly) > 0)
#define is_writable() ((openMode() & QIODevice::WriteOnly) > 0)

QFCgiStream::QFCgiStream(QObject *parent) : QIODevice(parent) {
  this->eof = false;
}

QFCgiStream::~QFCgiStream() {
}

bool QFCgiStream::atEnd() const {
  return is_readable() && this->eof && this->buffer.isEmpty();
}

qint64 QFCgiStream::bytesAvailable() const {
  return this->buffer.size() + QIODevice::bytesAvailable();
}

bool QFCgiStream::isSequential() const {
  return true;
}

QByteArray& QFCgiStream::getBuffer() {
  return this->buffer;
}

bool QFCgiStream::append(const QByteArray &ba) {
  if (is_readable() && !this->eof) {
    this->buffer.append(ba);
    emit readyRead();
    return true;
  } else {
    return false;
  }
}

bool QFCgiStream::setEof() {
  if (is_readable() && !this->eof) {
    this->eof = true;
    emit readChannelFinished();
    return true;
  } else {
    return false;
  }
}

qint64 QFCgiStream::readData(char *data, qint64 maxSize) {
  if (is_readable()) {
    if (this->buffer.isEmpty()) {
      return this->eof ? -1 : 0;
    }
    qint64 nbytes = qMin(this->buffer.size(), (int)maxSize);
    memcpy(data, this->buffer.data(), nbytes);
    this->buffer.remove(0, nbytes);

    return nbytes;
  } else {
    return -1;
  }
}

qint64 QFCgiStream::writeData(const char *data, qint64 maxSize) {
  if (is_writable()) {
    this->buffer.append(data, maxSize);
    emit bytesWritten(maxSize);

    return maxSize;
  } else {
    return -1;
  }
}
