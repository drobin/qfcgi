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

cmake_minimum_required(VERSION 2.6)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt4 REQUIRED QtCore QtNetwork)

if (NOT DISABLE_TESTING)
  enable_testing()
  add_subdirectory(test)
endif(NOT DISABLE_TESTING)

add_definitions(-Wall -Werror)

if (ENABLE_DEBUG)
  add_definitions(-g -O0)
endif(ENABLE_DEBUG)

add_library(qfcgi
  src/builder.h
  src/connection.cpp
  src/connection.h
  src/fcgi.cpp
  src/fcgi.h
  src/fdbuilder.cpp
  src/fdbuilder.h
  src/localbuilder.cpp
  src/localbuilder.h
  src/qfcgi.h
  src/record.cpp
  src/record.h
  src/request.cpp
  src/request.h
  src/stream.cpp
  src/stream.h
  src/tcpbuilder.cpp
  src/tcpbuilder.h
)

target_link_libraries(qfcgi Qt4::QtCore Qt4::QtNetwork)

install(TARGETS qfcgi
  ARCHIVE DESTINATION lib
)
install(FILES src/qfcgi.h
  DESTINATION include
)
install(FILES src/fcgi.h src/request.h
  DESTINATION include/qfcgi
)
