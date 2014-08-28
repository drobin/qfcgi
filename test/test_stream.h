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

#ifndef QFCGI_TEST_STREAM_H
#define QFCGI_TEST_STREAM_H

#include "../src/qfcgi/stream.h"

class TestStream : public QFCgiStream {
  Q_OBJECT

public:
  TestStream(QObject *parent = 0) : QFCgiStream(parent) {}
  ~TestStream() {}

public slots:
  void closeSlot() { this->close(); }
  void writeSlot() { this->write("\1\2\3", 3); }
  void setEofSlot() { this->setEof(); }
  void appendSlot() { this->append(QByteArray("\1\2\3", 3)); }
};

#endif  /* QFCGI_TEST_STREAM_H */
