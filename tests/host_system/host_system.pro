TARGET = host-system-test
SOURCES += host_system.cpp

QT += dbus
QT -=gui
TEMPLATE = app


INCLUDEPATH += ../../system ../../../system

#LIBS += -L../../system -L../../../system -lqmsystem2
equals(QT_MAJOR_VERSION, 4): QMAKE_LIBDIR_FLAGS += -L../../system/ -L../../../system -lqmsystem2
equals(QT_MAJOR_VERSION, 5): QMAKE_LIBDIR_FLAGS += -L../../system/ -L../../../system -lqmsystem2-qt5

equals(QT_MAJOR_VERSION, 4): target.path = /opt/tests/qmsystem-tests
equals(QT_MAJOR_VERSION, 5): target.path = /opt/tests/qmsystem-qt5-tests
INSTALLS += target


