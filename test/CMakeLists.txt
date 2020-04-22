##
# This file is part of QFCgi.
#
# QFCgi is free software: you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# QFCgi is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with QFCgi. If not, see <http://www.gnu.org/licenses/>.
##

add_executable(test_stream stream.cpp test_stream.h)
target_link_libraries(test_stream Qt5::Test qfcgi)

add_executable(test_record record.cpp)
target_link_libraries(test_record Qt5::Test qfcgi)

add_executable(test_request request.cpp)
target_link_libraries(test_request Qt5::Test qfcgi)

add_test(stream test_stream)
add_test(record test_record)
add_test(request test_request)
