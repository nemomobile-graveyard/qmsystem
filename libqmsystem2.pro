TEMPLATE = subdirs
SUBDIRS  = system keyd tests

PKGCONFIGFILES.files = qmsystem2-qt5.pc
PKGCONFIGFILES.path = /usr/lib/pkgconfig

QTCONFIGFILES.files = qmsystem2-qt5.prf
QTCONFIGFILES.path = /usr/share/qt5/mkspecs/features

INSTALLS += PKGCONFIGFILES QTCONFIGFILES
