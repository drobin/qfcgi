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

#ifndef QFCGI_CONNECTION_H
#define QFCGI_CONNECTION_H

#include <QHash>
#include <QObject>

class QFCgi;
class QFCgiRecord;
class QFCgiRequest;
class QIODevice;

class QFCgiConnection : public QObject {
  Q_OBJECT

public:
  QFCgiConnection(QIODevice *device, QFCgi *parent);
  virtual ~QFCgiConnection();

  int getId() const;

  void send(const QFCgiRecord &record);
  void closeConnection();

private slots:
  void onReadyRead();
  void onDisconnected();

private:
  void fillBuffer();
  void handleManagementRecord(QFCgiRecord &record);
  void handleApplicationRecord(QFCgiRecord &record);
  void handleFCGI_BEGIN_REQUEST(QFCgiRecord &record);
  void handleFCGI_PARAMS(QFCgiRequest *request, QFCgiRecord &record);
  void handleFCGI_STDIN(QFCgiRequest *request, QFCgiRecord &record);
  bool validateRole(quint16 role) const;

  int id;
  QIODevice *device;
  QByteArray buf;
  QHash<int, QFCgiRequest*> requests;
};

#endif  /* QFCGI_CONNECTION_H */
