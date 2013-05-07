TEMPLATE = subdirs
SUBDIRS  = system keyd tests

equals(QT_MAJOR_VERSION, 4): PKGCONFIGFILES.files = qmsystem2.pc
equals(QT_MAJOR_VERSION, 5): PKGCONFIGFILES.files = qmsystem2-qt5.pc
PKGCONFIGFILES.path = /usr/lib/pkgconfig

equals(QT_MAJOR_VERSION, 4): QTCONFIGFILES.files = qmsystem2.prf
equals(QT_MAJOR_VERSION, 5): QTCONFIGFILES.files = qmsystem2-qt5.prf
equals(QT_MAJOR_VERSION, 4): QTCONFIGFILES.path = /usr/share/qt4/mkspecs/features
equals(QT_MAJOR_VERSION, 5): QTCONFIGFILES.path = /usr/share/qt5/mkspecs/features

INSTALLS += PKGCONFIGFILES QTCONFIGFILES
