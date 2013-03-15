QT += dbus
QT -=gui

TARGET = manual-usbmode-test
SOURCES += manual_usbmode.cpp
LIBS += -lrt

include(../common-install.pri)
