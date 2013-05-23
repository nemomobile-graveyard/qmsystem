QT -= gui
SOURCES += devicemode.cpp
equals(QT_MAJOR_VERSION, 4): LIBS += -lQtDBus
equals(QT_MAJOR_VERSION, 5): LIBS += -lQt5DBus

TARGET = devicemode-test
include(../common-install.pri)
