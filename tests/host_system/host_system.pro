TARGET = host-system-test
SOURCES += host_system.cpp

QT += dbus
QT -=gui
TEMPLATE = app

INCLUDEPATH += ../../system ../../../system

QMAKE_LIBDIR_FLAGS += -L../../system/ -L../../../system -lqmsystem2-qt5

target.path = /opt/tests/qmsystem-qt5-tests
INSTALLS += target


