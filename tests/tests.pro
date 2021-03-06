equals(QT_MAJOR_VERSION, 4): system(cp tests-qt4.xml tests.xml)
equals(QT_MAJOR_VERSION, 5): system(cp tests-qt5.xml tests.xml)

TEMPLATE = subdirs

SUBDIRS = accelerometer \
          activity \
          als \
          cabc \
          callstate \
          compass \
          devicemode \
          displaystate \
          heartbeat \
          hw_keys \
          led \
          locks \
          orientation \
          proximity \
          rotation \
          magnetometer \
          system \
          systeminformation \
          systemsignals \
          tap \
          time \
          manual_orientation \
          manual_accelerometer \
          manual_batterymon \
          manual_bgcalibration \
          manual_als \
          manual_displaystate \
          manual_locks \
          manual_rotation \
          host_system \
          processwatchdog \
          manual_keys \
          manual_led \
          manual_proximity \
          manual_usbmode \
          manual_tap\
          usbmode \
	  powerontime

linux-g++-maemo {
    SUBDIRS += battery \
               thermal
}

# Test definition installation
testdefinition.files = tests.xml
equals(QT_MAJOR_VERSION, 4): testdefinition.path = /usr/share/qmsystem-tests
equals(QT_MAJOR_VERSION, 5): testdefinition.path = /usr/share/qmsystem-qt5-tests

INSTALLS += testdefinition
