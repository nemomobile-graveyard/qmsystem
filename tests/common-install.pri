QT += testlib
QT -= gui
TEMPLATE = app
QMAKE_CXXFLAGS += -Wall -Wno-psabi

INCLUDEPATH += ../../system ../../../system
LIBS += -lqmsystem2-qt5
QMAKE_LIBDIR_FLAGS += -L../../system -L../../../system 
target.path = /opt/tests/qmsystem-qt5-tests
INSTALLS += target
