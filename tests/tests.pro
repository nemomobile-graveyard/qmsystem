
TEMPLATE = subdirs

SUBDIRS = accelerometer \
          activity \
          als \
          battery \
          cabc \
          callstate \
          compass \
          devicemode \
          displaystate \
          heartbeat \
          hw_keys \
          led \
          orientation \
          proximity \
          rotation \
          magnetometer \
          system \
          systemsignals \
          tap \
          thermal \
          time \
          manual_orientation \
          manual_accelerometer \
          manual_bgcalibration \
          manual_compass \
          manual_als \
          manual_displaystate \
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

# Test definition installation
testdefinition.files = tests.xml session
testdefinition.path = /usr/share/qmsystem2-api-fast-tests

INSTALLS += testdefinition
