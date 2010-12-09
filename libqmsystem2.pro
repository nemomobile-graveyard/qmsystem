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

    message("Compiling with usb-moded-dev support")
    DEFINES += __USB_MODED_DEV__
    PKGCONFIG += usb_moded

    message("Compiling with sysinfo support")
    DEFINES += __SYSINFO__
    PKGCONFIG += sysinfo

    message("Compiling with mce support")
    DEFINES += __MCE__
} else {
    message("Compiling without devicelock support")
    message("Compiling without usb-moded-dev support")
    message("Compiling without sysinfo support")
    message("Compiling without mce support")
}
