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

#ifndef QFCGI_STREAM_H
#define QFCGI_STREAM_H

#include <QIODevice>

class QFCgiStream : public QIODevice {
  Q_OBJECT

public:
  QFCgiStream(QObject *parent = 0);
  virtual ~QFCgiStream();

  bool atEnd() const;
  qint64 bytesAvailable() const;
  bool isSequential() const;

  QByteArray& getBuffer();
  bool append(const QByteArray &ba);
  bool setEof();

protected:
  qint64 readData(char *data, qint64 maxSize);
  qint64 writeData(const char *data, qint64 maxSize);

private:
  QByteArray buffer;
  bool eof;
};

#endif  /* QFCGI_STREAM_H */
