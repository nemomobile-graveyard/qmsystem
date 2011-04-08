QT += dbus
QT -= gui
CONFIG += link_pkgconfig
PKGCONFIG += gconf-2.0
SOURCES += manual_compass.cpp

TARGET = manual-compass-test

include(../common-install.pri)
