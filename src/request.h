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

#ifndef QFCGI_REQUEST_H
#define QFCGI_REQUEST_H

#include <QHash>
#include <QObject>

class QBuffer;
class QFCgiConnection;
class QFCgiStream;

class QFCgiRequest : public QObject {
  Q_OBJECT

public:
  QFCgiRequest(int id, bool keepConn, QFCgiConnection *parent);

  int getId() const;
  bool keepConnection() const;

  QList<QString> getParams() const;
  QString getParam(const QString &name) const;

  QIODevice* getIn();

private slots:
  void onParamsReadyRead();

private:
  friend class QFCgiConnection;

  qint32 readNameValuePair(QString &name, QString &value);
  qint32 readLengthField(int pos, quint32 *length);
  qint32 readValueField(int pos, quint32 length, QString &value);

  int id;
  bool keepConn;
  QBuffer *paramsBuffer;
  QFCgiStream *in;
  QHash<QString, QString> params;
};

#endif  /* QFCGI_REQUEST_H */
