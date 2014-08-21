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

#ifndef QFCGI_TEST_RECORD_HELPER_H
#define QFCGI_TEST_RECORD_HELPER_H

QByteArray binaryRecord(quint8 version, quint8 type, quint16 requestId, const QByteArray &content) {
  QByteArray ba;
  quint16 contentLength = content.size();
  quint8 paddingLength;
  quint8 reserved = 0;

  paddingLength = contentLength % 8;
  paddingLength = (paddingLength > 0) ? 8 - paddingLength : 0;

  return QByteArray().append(version)
    .append(type)
    .append((requestId >> 8) & 0xFF).append(requestId & 0xFF)
    .append((contentLength >> 8) & 0xFF).append(contentLength & 0xFF)
    .append(paddingLength)
    .append(reserved)
    .append(content)
    .append(QByteArray(paddingLength, 0));
}

QByteArray binaryBeginRequest(quint16 requestId, quint16 role, bool keepConnection) {
  QByteArray content;
  const char reserved[5] = { 0 };

  content.append((role >> 8) & 0xFF).append(role & 0xFF)  // role
    .append(keepConnection ? 1 : 0)                       // flags
    .append(reserved);                                    // reserved

  return binaryRecord(1, 1, requestId, content);
}

QByteArray binaryParam(quint16 requestId, const QByteArray &params) {
  return binaryRecord(1, 4, requestId, params);
}

#endif  /* QFCGI_TEST_RECORD_HELPER_H */
