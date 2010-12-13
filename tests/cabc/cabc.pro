QT -= gui
SOURCES += cabc.cpp

TARGET = cabc-test

linux-g++-maemo {
    DEFINES += HAVE_SYSINFO
    PKGCONFIG += sysinfo
}

include(../common-install.pri)
