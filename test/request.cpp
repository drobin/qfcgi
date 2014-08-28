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

#include "../src/qfcgi/fcgi.h"
#include "../src/qfcgi/request.h"

#include "param_helper.h"
#include "record_helper.h"

class RequestTest: public QObject {
  Q_OBJECT

private slots:
  void initTestCase() {
    qRegisterMetaType<QFCgiRequest*>();
  }

  void init() {
    this->fcgi = new QFCgi(this);
    this->fcgi->configureListen(QHostAddress::LocalHost, 8000);
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

  void newRequestRoleAuthorizer() {
    QVERIFY(this->so->write(binaryBeginRequest(1, 2, 1)) > 0);

    QObject::connect(this->so, SIGNAL(readyRead()), loop, SLOT(quit()));
    while (this->so->bytesAvailable() < 16) {
      loop->exec();
    }

    verifyEndRequest(this->so, 1, 0, 3);
  }

  void newRequestRoleFilter() {
    QVERIFY(this->so->write(binaryBeginRequest(1, 3, 1)) > 0);

    QObject::connect(this->so, SIGNAL(readyRead()), loop, SLOT(quit()));
    while (this->so->bytesAvailable() < 16) {
      loop->exec();
    }

    verifyEndRequest(this->so, 1, 0, 3);
  }

  void newRequestRoleUnknown() {
    QVERIFY(this->so->write(binaryBeginRequest(1, 4, 0)) > 0); /* keep_conn = false, connection is closed anyway */

    QObject::connect(this->so, SIGNAL(disconnected()), loop, SLOT(quit()));
    loop->exec();

    QVERIFY(this->so->bytesAvailable() == 16);
    verifyEndRequest(this->so, 1, 0, 3);
  }

  void newRequestInvalidRequestId() {
    QVERIFY(this->so->write(binaryBeginRequest(1, 1, 0)) > 0);
    QVERIFY(this->so->write(binaryBeginRequest(1, 1, 0)) > 0); // a second request with the same request-id

    QObject::connect(this->so, SIGNAL(disconnected()), loop, SLOT(quit()));
    loop->exec();

    QVERIFY(this->so->bytesAvailable() == 16);
    verifyEndRequest(this->so, 1, 0, 2);
  }

  void newRequestNoParams() {
    QVERIFY(this->so->write(binaryBeginRequest(1, 1, 0)) > 0);
    QVERIFY(this->so->write(binaryParam(1, QByteArray())) > 0);

    QSignalSpy spy(this->fcgi, SIGNAL(newRequest(QFCgiRequest*)));
    QObject::connect(this->fcgi, SIGNAL(newRequest(QFCgiRequest*)), loop, SLOT(quit()));
    loop->exec();

    QFCgiRequest *request = qvariant_cast<QFCgiRequest*>(spy.at(0).at(0));
    QVERIFY(request != 0);

    QCOMPARE(request->getParams().count(), 0);

    request->endRequest(0);
  }

  void newRequestParamsOneRecord() {
    QVERIFY(this->so->write(binaryBeginRequest(1, 1, 0)) > 0);

    QByteArray params = encodeParam("k1", "v1").append(encodeParam("k2", "v2"));
    QVERIFY(this->so->write(binaryParam(1, params)) > 0);
    QVERIFY(this->so->write(binaryParam(1, QByteArray())) > 0);

    QSignalSpy spy(this->fcgi, SIGNAL(newRequest(QFCgiRequest*)));
    QObject::connect(this->fcgi, SIGNAL(newRequest(QFCgiRequest*)), loop, SLOT(quit()));
    loop->exec();

    QFCgiRequest *request = qvariant_cast<QFCgiRequest*>(spy.at(0).at(0));
    QVERIFY(request != 0);

    QCOMPARE(request->getParams().count(), 2);
    QCOMPARE(request->getParam("k1"), QString("v1"));
    QCOMPARE(request->getParam("k2"), QString("v2"));

    request->endRequest(0);
  }

  void newRequestParamsOneRecordBigKey() {
    const QString bigKey = bigString("abc", 44);
    QVERIFY(this->so->write(binaryBeginRequest(1, 1, 0)) > 0);

    QByteArray params = encodeParam(bigKey, "v1").append(encodeParam("k2", "v2"));
    QVERIFY(this->so->write(binaryParam(1, params)) > 0);
    QVERIFY(this->so->write(binaryParam(1, QByteArray())) > 0);

    QSignalSpy spy(this->fcgi, SIGNAL(newRequest(QFCgiRequest*)));
    QObject::connect(this->fcgi, SIGNAL(newRequest(QFCgiRequest*)), loop, SLOT(quit()));
    loop->exec();

    QFCgiRequest *request = qvariant_cast<QFCgiRequest*>(spy.at(0).at(0));
    QVERIFY(request != 0);

    QCOMPARE(request->getParams().count(), 2);
    QCOMPARE(request->getParam(bigKey), QString("v1"));
    QCOMPARE(request->getParam("k2"), QString("v2"));

    request->endRequest(0);
  }

  void newRequestParamsOneRecordBigValue() {
    const QString bigValue = bigString("abc", 44);
    QVERIFY(this->so->write(binaryBeginRequest(1, 1, 0)) > 0);

    QByteArray params = encodeParam("k1", bigValue).append(encodeParam("k2", "v2"));
    QVERIFY(this->so->write(binaryParam(1, params)) > 0);
    QVERIFY(this->so->write(binaryParam(1, QByteArray())) > 0);

    QSignalSpy spy(this->fcgi, SIGNAL(newRequest(QFCgiRequest*)));
    QObject::connect(this->fcgi, SIGNAL(newRequest(QFCgiRequest*)), loop, SLOT(quit()));
    loop->exec();

    QFCgiRequest *request = qvariant_cast<QFCgiRequest*>(spy.at(0).at(0));
    QVERIFY(request != 0);

    QCOMPARE(request->getParams().count(), 2);
    QCOMPARE(request->getParam("k1"), bigValue);
    QCOMPARE(request->getParam("k2"), QString("v2"));

    request->endRequest(0);
  }

  void newRequestParamsTwoRecords() {
    QVERIFY(this->so->write(binaryBeginRequest(1, 1, 0)) > 0);

    QByteArray params = encodeParam("k1", "v123456").append(encodeParam("k2", "v2"));
    QVERIFY(this->so->write(binaryParam(1, params.left(params.count() / 2))) > 0);
    QVERIFY(this->so->write(binaryParam(1, params.mid(params.count() / 2))) > 0);
    QVERIFY(this->so->write(binaryParam(1, QByteArray())) > 0);

    QSignalSpy spy(this->fcgi, SIGNAL(newRequest(QFCgiRequest*)));
    QObject::connect(this->fcgi, SIGNAL(newRequest(QFCgiRequest*)), loop, SLOT(quit()));
    loop->exec();

    QFCgiRequest *request = qvariant_cast<QFCgiRequest*>(spy.at(0).at(0));
    QVERIFY(request != 0);

    QCOMPARE(request->getParams().count(), 2);
    QCOMPARE(request->getParam("k1"), QString("v123456"));
    QCOMPARE(request->getParam("k2"), QString("v2"));

    request->endRequest(0);
  }

private:
  QFCgi *fcgi;
  QTcpSocket *so;
  QEventLoop *loop;

  QString bigString(const QString &in, int count) {
    QByteArray ba;

    for (int i = 0; i < count; i++) {
      ba.append(in);
    }

    return ba;
  }
};

QTEST_MAIN(RequestTest)
#include "request.moc"
