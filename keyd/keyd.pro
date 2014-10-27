# -------------------------------------------------
# Project created by QtCreator 2010-03-04T09:54:42
# -------------------------------------------------
QT += network
QT -= gui
TARGET = qmkeyd2-qt5
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    qmkeyd.cpp \
    keytranslator.cpp
HEADERS += qmkeyd.h \
    keytranslator.h
LIBS += -lrt

target.path = $$(DESTDIR)/usr/sbin
INSTALLS = target
