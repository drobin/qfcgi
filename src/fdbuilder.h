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

#ifndef QFCGI_FD_BUILDER_H
#define QFCGI_FD_BUILDER_H

#include "builder.h"

class QSocketNotifier;

class QFCgiFdConnectionBuilder : public QFCgiConnectionBuilder {
  Q_OBJECT

public:
  QFCgiFdConnectionBuilder(int fd,QObject *parent = 0);
  virtual ~QFCgiFdConnectionBuilder();

  bool listen();
  bool isListening() const;
  QString errorString() const;

private slots:
  void onActivated(int socket);

private:
  QSocketNotifier *notifier;
  int fd;
};

#endif  /* QFCGI_FD_BUILDER_H */
