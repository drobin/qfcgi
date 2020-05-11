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
find_package(Qt5 REQUIRED Core Network Test)

if (NOT DISABLE_TESTING)
  enable_testing()
  add_subdirectory(test)
endif(NOT DISABLE_TESTING)

if (NOT DISABLE_EXAMPLE)
  add_subdirectory(example)
endif(NOT DISABLE_EXAMPLE)

add_definitions(-Wall -Werror)

set(ENABLE_DEBUG ON)

if (ENABLE_DEBUG)
  add_definitions(-g -O0)
else(ENABLE_DEBUG)
  add_definitions(-DQT_NO_DEBUG_OUTPUT -DQT_NO_WARNING_OUTPUT)
endif(ENABLE_DEBUG)

add_library(qfcgi
  src/qfcgi.h
  src/qfcgi/builder.h
  src/qfcgi/connection.cpp
  src/qfcgi/connection.h
  src/qfcgi/fcgi.cpp
  src/qfcgi/fcgi.h
  src/qfcgi/fdbuilder.cpp
  src/qfcgi/fdbuilder.h
  src/qfcgi/localbuilder.cpp
  src/qfcgi/localbuilder.h
  src/qfcgi/record.cpp
  src/qfcgi/record.h
  src/qfcgi/request.cpp
  src/qfcgi/request.h
  src/qfcgi/stream.cpp
  src/qfcgi/stream.h
  src/qfcgi/tcpbuilder.cpp
  src/qfcgi/tcpbuilder.h
)

target_link_libraries(qfcgi Qt5::Core Qt5::Network)

install(TARGETS qfcgi
  ARCHIVE DESTINATION lib
)
install(FILES src/qfcgi.h
  DESTINATION include
)
install(FILES src/qfcgi/fcgi.h src/qfcgi/request.h
  DESTINATION include/qfcgi
)
