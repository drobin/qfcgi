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

#include <QCoreApplication>
#include <QHostAddress>
#include <QTimer>

#include <signal.h>
#include <qfcgi.h>

class QFCgiApp : public QCoreApplication {
  Q_OBJECT

public:
  QFCgiApp(int argc, char *argv[]) : QCoreApplication(argc, argv) {
    this->fcgi = new QFCgi(this);

    // Connect to the newRequest signal, the onNewRequest() method
    // is invoked every time a new FastCGI request is available
    connect(this->fcgi, SIGNAL(newRequest(QFCgiRequest*)),
            this, SLOT(onNewRequest(QFCgiRequest*)));

    // confiure the FastCGI application server to listen on localhost, port 9000
    this->fcgi->configureListen(QHostAddress::LocalHost, 9000);

    // start the FastCGI application server
    this->fcgi->start();

    // check the status of the FastCGI application server
    if (!this->fcgi->isStarted()) {
      qCritical() << this->fcgi->errorString();
      QTimer::singleShot(0, this, SLOT(quit()));
    }
  }

private slots:
  void onNewRequest(QFCgiRequest *request) {
    // You have a new request, this sample implementation will write back
    // a list of all received parameter.

    // request->getOut() is a stream which is used to write back information
    // to the webserver.
    QTextStream ts(request->getOut());

    ts << "Content-Type: plain/text\n";
    ts << "\n";
    ts << QString("Hello from %1\n").arg(this->applicationName());
    ts << "This is what I received:\n";

    Q_FOREACH(QString key, request->getParams()) {
      ts << QString("%1: %2\n").arg(key).arg(request->getParam(key));
    }

    ts.flush();

    // Don't forget to call endRequest() to finalize the request
    request->endRequest(0);
  }

private:
  QFCgi *fcgi;
};

static void quit_handler(int signal) {
  qApp->quit();
}

int main(int argc, char *argv[]) {
  QFCgiApp app(argc, argv);
  struct sigaction sa;

  memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = quit_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  if (sigaction(SIGHUP, &sa, 0) != 0 ||
      sigaction(SIGTERM, &sa, 0) != 0 ||
      sigaction(SIGINT, &sa, 0) != 0) {
    perror(argv[0]);
    return 1;
  }

  return app.exec();
}

#include "qfcgiapp.moc"
