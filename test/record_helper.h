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

void verifyEnvelope(QIODevice *dev, quint8 type, quint16 requestId, quint16 *contentLength, quint8 *paddingLength) {
  char data[8] = { 0 };

  QVERIFY(dev->read(data, sizeof(data)) == 8);

  QVERIFY(data[0] == 1); // version
  QVERIFY((data[1] & 0xFF) == type); // type
  QVERIFY((((data[2] & 0xFF) << 8) | (data[3] & 0xFF)) == requestId); // request id

  *contentLength = ((data[4] & 0xFF) << 8) | (data[5] & 0xFF);
  *paddingLength = data[6] & 0xFF;
  // data[7] := reserved
}

void verifyEndRequest(QIODevice *dev, quint16 requestId, quint32 appStatus, quint8 protocolStatus) {
  quint16 contentLength;
  quint8 paddingLength;
  quint32 u32;

  verifyEnvelope(dev, 3, requestId, &contentLength, &paddingLength);
  QVERIFY(contentLength == 8);
  QVERIFY(paddingLength == 0);

  char content[8];
  QVERIFY(dev->read(content, sizeof(content)) == 8);
  u32 = ((content[0] & 0xFF) << 24) | (content[1] & 0xFF) << 16 |
    ((content[2] & 0xFF) << 8) | (content[3] & 0xFF);
  QVERIFY(appStatus == u32);
  QVERIFY((content[4] & 0xFF) == protocolStatus);
}

#endif  /* QFCGI_TEST_RECORD_HELPER_H */
