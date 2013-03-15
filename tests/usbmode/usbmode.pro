QT += dbus
QT -=gui

TARGET = usbmode-test
SOURCES += usbmode.cpp
LIBS += -lrt

include(../common-install.pri)
