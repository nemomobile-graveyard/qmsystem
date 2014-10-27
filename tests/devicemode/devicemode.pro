QT -= gui
SOURCES += devicemode.cpp
LIBS += -lQt5DBus

TARGET = devicemode-test
include(../common-install.pri)
