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

#include <QtTest/QtTest>

#include "../src/qfcgi/record.h"

#include "record_helper.h"

class RecordTest: public QObject {
  Q_OBJECT

private slots:
  void init() {
    this->record = new QFCgiRecord;
    this->buffer = new QBuffer;

    QVERIFY(this->buffer->open(QBuffer::ReadWrite));
  }

  void cleanup() {
    delete this->record;
    delete this->buffer;
  }

  void getVersion() {
    QVERIFY(record->getVersion() == QFCgiRecord::V1);
  }

  void getType() {
    QVERIFY(record->getType() == QFCgiRecord::FCGI_UNKNOWN_TYPE);
  }

  void setType() {
    record->setType(QFCgiRecord::FCGI_PARAMS);
    QVERIFY(record->getType() == QFCgiRecord::FCGI_PARAMS);
  }

  void getRequestId() {
    QVERIFY(record->getRequestId() == 0);
  }

  void setRequestId() {
    record->setRequestId(4711);
    QVERIFY(record->getRequestId() == 4711);
  }

  void getContent() {
    QVERIFY(record->getContent().size() == 0);
  }

  void readInvalidVersion() {
    QVERIFY(record->read(binaryRecord(9, 1, 0, QByteArray())) == -1);
  }

  void readInvalidType() {
    QVERIFY(record->read(binaryRecord(1, 12, 0, QByteArray())) == -1);
  }

  void readNoContent() {
    QVERIFY(record->read(binaryRecord(1, 2, 3, QByteArray())) == 8);
    QVERIFY(record->getVersion() == QFCgiRecord::V1);
    QVERIFY(record->getType() == QFCgiRecord::FCGI_ABORT_REQUEST);
    QVERIFY(record->getRequestId() == 3);
    QVERIFY(record->getContent().size() == 0);
  }

  void readEmptyPadding() {
    QVERIFY(record->read(binaryRecord(1, 2, 3, QByteArray("12345678", 8))) == 16);
    QVERIFY(record->getVersion() == QFCgiRecord::V1);
    QVERIFY(record->getType() == QFCgiRecord::FCGI_ABORT_REQUEST);
    QVERIFY(record->getRequestId() == 3);
    QVERIFY(record->getContent() == QByteArray("12345678", 8));
  }

  void readWithPadding() {
    QVERIFY(record->read(binaryRecord(1, 2, 3, QByteArray("12345", 5))) == 16);
    QVERIFY(record->getVersion() == QFCgiRecord::V1);
    QVERIFY(record->getType() == QFCgiRecord::FCGI_ABORT_REQUEST);
    QVERIFY(record->getRequestId() == 3);
    QVERIFY(record->getContent() == QByteArray("12345", 5));
  }

  void writeEmptyContent() {
    QVERIFY(record->write(this->buffer) == 8);
    QVERIFY(buffer->buffer() == binaryRecord(1, 11, 0, QByteArray()));
  }

  void createEndRequest() {
    QFCgiRecord r = QFCgiRecord::createEndRequest(99, 1, QFCgiRecord::FCGI_OVERLOADED);
    QVERIFY(r.write(buffer) == 16);
    QVERIFY(buffer->buffer() == binaryRecord(1, 3, 99, QByteArray("\0\0\0\1\2\0\0\0", 8)));
  }

  void createOutStreamWithData() {
    QFCgiRecord r = QFCgiRecord::createOutStream(99, QByteArray("123", 3));
    QVERIFY(r.write(buffer) == 16);
    QVERIFY(buffer->buffer() == binaryRecord(1, 6, 99, QByteArray("123", 3)));
  }

  void createOutStreamEof() {
    QFCgiRecord r = QFCgiRecord::createOutStream(99, QByteArray());
    QVERIFY(r.write(buffer) == 8);
    QVERIFY(buffer->buffer() == binaryRecord(1, 6, 99, QByteArray()));
  }

  void createErrStreamWithData() {
    QFCgiRecord r = QFCgiRecord::createErrStream(99, QByteArray("123", 3));
    QVERIFY(r.write(buffer) == 16);
    QVERIFY(buffer->buffer() == binaryRecord(1, 7, 99, QByteArray("123", 3)));
  }

  void createErrStreamEof() {
    QFCgiRecord r = QFCgiRecord::createErrStream(99, QByteArray());
    QVERIFY(r.write(buffer) == 8);
    QVERIFY(buffer->buffer() == binaryRecord(1, 7, 99, QByteArray()));
  }

  void createDataStreamWithData() {
    QFCgiRecord r = QFCgiRecord::createDataStream(99, QByteArray("123", 3));
    QVERIFY(r.write(buffer) == 16);
    QVERIFY(buffer->buffer() == binaryRecord(1, 8, 99, QByteArray("123", 3)));
  }

  void createDataStreamEof() {
    QFCgiRecord r = QFCgiRecord::createDataStream(99, QByteArray());
    QVERIFY(r.write(buffer) == 8);
    QVERIFY(buffer->buffer() == binaryRecord(1, 8, 99, QByteArray()));
  }

private:
  QFCgiRecord *record;
  QBuffer *buffer;
};

QTEST_MAIN(RecordTest)
#include "record.moc"
