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

#ifndef QFCGI_TEST_PARAM_HELPER_H
#define QFCGI_TEST_PARAM_HELPER_H

QByteArray encodeLength(const QString &s) {
  int len = s.toUtf8().length();

  if (len <= 127) {
    return QByteArray().append(len);
  } else {
    return QByteArray()
      .append(((len >> 24) & 0xFF) | 0x80)
      .append((len >> 16) & 0xFF)
      .append((len >> 8) & 0xFF)
      .append(len & 0xFF);
  }
}

QByteArray encodeValue(const QString &s) {
  return s.toUtf8();
}

QByteArray encodeParam(const QString &key, const QString &value) {
  return QByteArray().append(encodeLength(key)).append(encodeLength(value))
    .append(encodeValue(key)).append(encodeValue(value));
}

#endif  /* QFCGI_TEST_PARAM_HELPER_H */
