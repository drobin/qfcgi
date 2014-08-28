QFCgi
=====

[FastCGI][1] implementation for the [Qt project][2]. The library utilizes the
Qt event pattern, it runs completely asynchronous. It hides (most) of the
FastCGI specification behind a simple interface.

Usage
-----

A common pattern can look like this:

    #include <qfcgi.h>                                      // (1)

    QFCgi *fcgi = new QFCgi;                                // (2)
    fcgi->configureListen(QHostAddress::Any, 9000);         // (3)
    fcgi->start();                                          // (4)
    connect(fcgi, SIGNAL(newRequest(QFCgiRequest*)), ...);  // (5)

1. Include the [`qfcgi.h`](src/qfcgi.h) header.
2. Create an instance of the [`QFCgi`](src/fcgi.h) class.
3. Configure the listener. There are several listener available.
4. Start the FastCGI system.
5. Wait for incoming requests.

Have a look into the [example directory](example/) where you can get through
some real examples.

Prerequisites
-------------

Developed and tested against [Qt V4.8](http://qt-project.org/doc/qt-4.8/).

Installation
------------

[CMake][3] is used as the build system:

    mkdir build
    cd build
    cmake ..
    make
    make install

Licence
-------

See [LICENSE](LICENSE) file.

[1]: http://www.fastcgi.com "FastCGI"
[2]: http://www.qt-project.org "Qt Project"
[3]: http://www.cmake.org "CMake"
