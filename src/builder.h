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

#ifndef QFCGI_BUILDER_H
#define QFCGI_BUILDER_H

#include <QObject>

class QFCgiConnection;

class QFCgiConnectionBuilder : public QObject {
  Q_OBJECT

public:
  QFCgiConnectionBuilder(QObject *parent = 0) : QObject(parent) {}
  virtual ~QFCgiConnectionBuilder() {}

  virtual bool listen() = 0;
  virtual bool isListening() const = 0;
  virtual QString errorString() const = 0;

signals:
  void newConnection(QFCgiConnection *connection);
};

#endif  /* QFCGI_BUILDER_H */
