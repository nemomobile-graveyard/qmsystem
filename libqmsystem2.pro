TEMPLATE = subdirs
SUBDIRS  = system keyd tests

include( doc/doc.pri )

PKGCONFIGFILES.files = qmsystem2.pc
PKGCONFIGFILES.path = /usr/lib/pkgconfig

QTCONFIGFILES.files = qmsystem2.prf
QTCONFIGFILES.path = /usr/share/qt4/mkspecs/features

INSTALLS += PKGCONFIGFILES QTCONFIGFILES

# for compiling on meego
linux-g++-maemo {
    message("Compiling with devicelock support")
    DEFINES += __DEVICELOCK__
} else {
    message("Compiling without devicelock support")
}
