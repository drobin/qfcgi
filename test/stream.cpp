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

#include "test_stream.h"
#include "../src/qfcgi/stream.h"

class StreamTest: public QObject {
  Q_OBJECT

private slots:
  void init() {
    this->stream = new TestStream(this);
    this->loop = new QEventLoop(this);

    QVERIFY(this->stream->open(QIODevice::ReadWrite));
  }

  void cleanup() {
    delete this->stream;
    delete this->loop;
  }

  void isSequential() {
    QVERIFY(stream->isSequential());
  }

  void aboutToCloseSignal() {
    QTimer::singleShot(0, stream, SLOT(closeSlot()));
    QObject::connect(stream, SIGNAL(aboutToClose()), loop, SLOT(quit()));
    loop->exec();
  }

  void bytesWrittenSignal() {
    QTimer::singleShot(0, stream, SLOT(writeSlot()));
    QObject::connect(stream, SIGNAL(bytesWritten(qint64)), loop, SLOT(quit()));
    loop->exec();
  }

  void readChannelFinishedSignal() {
    QTimer::singleShot(0, stream, SLOT(setEofSlot()));
    QObject::connect(stream, SIGNAL(readChannelFinished()), loop, SLOT(quit()));
    loop->exec();
  }

  void readyReadSignal() {
    QTimer::singleShot(0, stream, SLOT(appendSlot()));
    QObject::connect(stream, SIGNAL(readyRead()), loop, SLOT(quit()));
    loop->exec();
  }

  void append() {
    QVERIFY(stream->append(QByteArray("123")));
    QCOMPARE(stream->getBuffer().size(), 3);
    QVERIFY(memcmp(stream->getBuffer().data(), "123", 3) == 0);
  }

  void appendNotOpen() {
    stream->close();
    QVERIFY(!stream->append(QByteArray("123")));
    QCOMPARE(stream->getBuffer().size(), 0);
  }

  void appendAtEof() {
    QVERIFY(stream->setEof());
    QVERIFY(!stream->append(QByteArray("123")));
    QCOMPARE(stream->getBuffer().size(), 0);
  }

  void setEof() {
    QVERIFY(stream->setEof());
    QVERIFY(stream->atEnd());
  }

  void setEofNotOpen() {
    stream->close();
    QVERIFY(!stream->setEof());
  }

  void atEndNotOpen() {
    stream->close();
    QVERIFY(!stream->atEnd());
  }

  void atEndBufferedData() {
    QVERIFY(stream->append(QByteArray("123")));
    QVERIFY(!stream->atEnd());
  }

  void atEndBufferedDataEof() {
    QVERIFY(stream->append(QByteArray("123")));
    QVERIFY(stream->setEof());
    QVERIFY(!stream->atEnd());
  }

  void atEndEof() {
    QVERIFY(stream->setEof());
    QVERIFY(stream->atEnd());
  }

  void bytesAvailable() {
    QVERIFY(stream->bytesAvailable() == 0);
    QVERIFY(stream->append(QByteArray("123")));
    QVERIFY(stream->bytesAvailable() == 3);
  }

  void read() {
    char data[16] = { 0 };
    QVERIFY(stream->append(QByteArray("123")));
    QVERIFY(stream->read(data, sizeof(data)) == 3);
    QVERIFY(memcmp(data, "123", 3) == 0);
  }

  void readNoData() {
    char data[16] = { 0 };
    QVERIFY(stream->read(data, sizeof(data)) == 0);
  }

  void readNotOpen() {
    char data[16] = { 0 };
    stream->close();
    QVERIFY(stream->read(data, sizeof(data)) == -1);
  }

  void readEof() {
    char data[16] = { 0 };
    QVERIFY(stream->setEof());
    QVERIFY(stream->read(data, sizeof(data)) == -1);
  }

  void write() {
    QVERIFY(stream->write("123") == 3);
    QCOMPARE(stream->getBuffer().size(), 3);
    QVERIFY(memcmp(stream->getBuffer().data(), "123", 3) == 0);
  }

  void writeNotOpen() {
    stream->close();
    QVERIFY(stream->write("123") == -1);
  }

private:
  TestStream *stream;
  QEventLoop *loop;
};

QTEST_MAIN(StreamTest)
#include "stream.moc"
