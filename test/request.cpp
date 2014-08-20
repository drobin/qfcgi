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
#include <QHostAddress>
#include <QTcpSocket>

#include "../src/fcgi.h"

#include "record_helper.h"

class RequestTest: public QObject {
  Q_OBJECT

private slots:
  void init() {
    this->fcgi = new QFCgi(this);
    this->fcgi->configureListen(QHostAddress::Any, 8000);
    this->fcgi->start();
    QVERIFY(this->fcgi->isStarted());

    this->so = new QTcpSocket(this);
    this->so->connectToHost("127.0.0.1", 8000);
    QVERIFY(this->so->waitForConnected());

    this->loop = new QEventLoop(this);
  }

  void cleanup() {
    delete this->fcgi;
    delete this->so;
    delete this->loop;
  }

  void closeConnectionOnInvalidRecord() {
    QVERIFY(this->so->write(binaryRecord(99, 2, 3, QByteArray())) > 0);

    QObject::connect(this->so, SIGNAL(disconnected()), loop, SLOT(quit()));
    loop->exec();
  }

  void closeConnectionOnInvalidRequestId() {
    QVERIFY(this->so->write(binaryRecord(1, 2, 3, QByteArray())) > 0);

    QObject::connect(this->so, SIGNAL(disconnected()), loop, SLOT(quit()));
    loop->exec();
  }

private:
  QFCgi *fcgi;
  QTcpSocket *so;
  QEventLoop *loop;
};

QTEST_MAIN(RequestTest)
#include "request.moc"
