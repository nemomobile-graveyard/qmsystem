QT += dbus
QT -= gui
CONFIG += link_pkgconfig
PKGCONFIG += gconf-2.0
SOURCES += manual_compass.cpp
LIBS += -lgobject-2.0

TARGET = manual-compass-test

include(../common-install.pri)
