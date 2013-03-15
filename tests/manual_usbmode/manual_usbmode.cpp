#include <QTime>
#include <QObject>
#include <qmusbmode.h>
#include <QTest>
#include <QStack>
#include <QDebug>
#include <iostream>

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

using namespace MeeGo;

typedef QStack<QmUSBMode::Mode> ModeStack;

class TestClass : public QObject
{
    Q_OBJECT

public slots:
    void modeChanged(MeeGo::QmUSBMode::Mode mode) {
        qDebug() << "Received a modeChanged signal: " << mode2str(mode);
        currentMode = mode;
        if (currentMode == QmUSBMode::MassStorage) {
            QmUSBMode::MountOptionFlags mountOptions = qmmode->mountStatus(QmUSBMode::DocumentDirectoryMount);
            qDebug() << "\n\nReadOnlyMount " << mountOptions.testFlag(QmUSBMode::ReadOnlyMount);
            qDebug() << "\n\nReadWriteMount " << mountOptions.testFlag(QmUSBMode::ReadWriteMount);
            verifyMountStatus();
            QVERIFY(unmountReceived);

        }

        if (currentMode == QmUSBMode::OviSuite) {
            QmUSBMode::MountOptionFlags mountOptions = qmmode->mountStatus(QmUSBMode::DocumentDirectoryMount);
            qDebug() << "\n\nReadOnlyMount " << mountOptions.testFlag(QmUSBMode::ReadOnlyMount);
            qDebug() << "\n\nReadWriteMount " << mountOptions.testFlag(QmUSBMode::ReadWriteMount);
            verifyMountStatus();
        }

        signalReceived = true;
        modeStack.push(mode);
    }

    void fileSystemWillUnmount(MeeGo::QmUSBMode::MountPath mountPath) {
        qDebug() << "\nReceived a fileSystemWillUnmount signal: " << mountPath;
        qDebug() << "Mount Path: " << mountPath;
        unmountReceived = true;
    }

    void error(const QString &errorCode) {
        qDebug() << "Received an ERROR signal: " << errorCode;
        signalReceived = true;
    }

private:
    QmUSBMode *qmmode;
    QmUSBMode::Mode currentMode;
    bool signalReceived;
    bool unmountReceived;
    ModeStack modeStack;

    void verifyMountStatus(bool mustBeWritableMount = false) {
        struct timespec ts = { 0, 0 };
        char *path = 0;
        FILE *f;
        QmUSBMode::MountOptionFlags mountOptions = qmmode->mountStatus(QmUSBMode::DocumentDirectoryMount);
        bool readWriteMount = (mountOptions & QmUSBMode::ReadWriteMount);
        bool readOnlyMount = (mountOptions & QmUSBMode::ReadOnlyMount);

        QVERIFY(clock_gettime(CLOCK_MONOTONIC, &ts) == 0);
        QVERIFY(asprintf(&path, "/home/user/MyDocs/qmsystem.usbmode-test.%lu.%lu",
                         (unsigned long)ts.tv_sec,
                         (unsigned long)ts.tv_nsec) != -1);

        f = fopen(path, "a+");
        if (f) {
            QVERIFY(readWriteMount);
            QVERIFY(!readOnlyMount);
            fclose(f);
        } else {
            if (access("/home/user/MyDocs", R_OK) == 0) {
                QVERIFY(readOnlyMount);
                QVERIFY(!readWriteMount);
            } else {
                QVERIFY(!readOnlyMount);
                QVERIFY(!readWriteMount);
            }
        }

        if (mustBeWritableMount) {
            QVERIFY(readWriteMount);
        }

        free(path);
    }

    QString mode2str(QmUSBMode::Mode mode) {
        switch (mode) {
        case QmUSBMode::Connected:
            return "Connected";
        case QmUSBMode::DataInUse:
            return "DataInUse";
        case QmUSBMode::Disconnected:
            return "Disconnected";
        case QmUSBMode::MassStorage:
            return "MassStorage";
        case QmUSBMode::ChargingOnly:
            return "ChargingOnly";
        case QmUSBMode::OviSuite:
            return "OviSuite";
        case QmUSBMode::ModeRequest:
            return "ModeRequest";
        case QmUSBMode::Ask:
            return "Ask";
        case QmUSBMode::SDK:
            return "SDK";
        case QmUSBMode::Undefined:
            return "Undefined";
	 case QmUSBMode::MTP:
	     return "MTP";
	 case QmUSBMode::Developer:
	     return "Developer";
        default:
            return "An unknown mode!";
        }
    }

    void testFunc(QmUSBMode::Mode defaultMode, QmUSBMode::Mode mode) {
        printf("\n\nTesting with defaultMode %s and mode %s\n\n", mode2str(defaultMode).toAscii().data(), mode2str(mode).toAscii().data());

        printf("\n\nPlease remove the usb cable.\n");
        printf("You have 10 seconds...\n\n");
        QTest::qWait(11*1000);
        QVERIFY(currentMode == QmUSBMode::Disconnected ||
                currentMode == QmUSBMode::Undefined);

        QmUSBMode::MountOptionFlags mountOptions = qmmode->mountStatus(QmUSBMode::DocumentDirectoryMount);
        verifyMountStatus();

        signalReceived = false;
        unmountReceived = false;
        currentMode = QmUSBMode::Undefined;
        QVERIFY(qmmode->setDefaultMode(defaultMode));
        QCOMPARE(qmmode->getDefaultMode(), defaultMode);

        printf("\n\nPlease plug in the usb cable.\n");
        printf("You have 10 seconds...\n\n");
        for (int i = 0; i < 111; i++) {
            QTest::qWait(1000);
            if (currentMode == defaultMode) {
                printf("Current mode equals to DefaultMode\n\n");
                break;
            }
        }
        QVERIFY(!modeStack.isEmpty());
        QCOMPARE(modeStack.pop(), defaultMode);
        QVERIFY(!modeStack.isEmpty());
        if (defaultMode == QmUSBMode::Ask) {
            QCOMPARE(modeStack.pop(), QmUSBMode::ModeRequest);
        } else if (defaultMode == QmUSBMode::MassStorage) {
            QCOMPARE(modeStack.pop(), QmUSBMode::DataInUse);
        }
        QVERIFY(!modeStack.isEmpty());
        QCOMPARE(modeStack.pop(), QmUSBMode::Connected);

        if (defaultMode == QmUSBMode::Ask) {
            QTime now;
            now.start();
            QVERIFY(qmmode->setMode(mode));
            qDebug("Time elapsed after setMode returns: %d ms", now.elapsed());
            QVERIFY(now.elapsed()<18000);
            qDebug("Waiting for SIGNALS for 6s");
            QTest::qWait(6000);
            QCOMPARE(currentMode, mode);
            QCOMPARE(qmmode->getMode(), currentMode);
        }

    }

private slots:
    void initTestCase() {
        qmmode = new QmUSBMode();
        QVERIFY(qmmode);
        QVERIFY(connect(qmmode, SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode)),
                        this, SLOT(modeChanged(MeeGo::QmUSBMode::Mode))));
        QVERIFY(connect(qmmode, SIGNAL(fileSystemWillUnmount(MeeGo::QmUSBMode::MountPath)),
                        this, SLOT(fileSystemWillUnmount(MeeGo::QmUSBMode::MountPath))));
        QVERIFY(connect(qmmode, SIGNAL(error(const QString)),
                        this, SLOT(error(const QString))));
    }

    void testAskOvi() {
        printf("This test will ask you to plug the usb cable in and out.\n");
        printf("This means that you will need to run this via serial cable. :)\n");
        printf("The test starts in 10 seconds...\n");
        QTest::qWait(10*1000);
        testFunc(QmUSBMode::Ask, QmUSBMode::OviSuite);
    }
    void testAskMass() {
        printf("This test will ask you to plug the usb cable in and out.\n");
        printf("This means that you will need to run this via serial cable. :)\n");
        printf("The test starts in 10 seconds...\n");
        QTest::qWait(10*1000);
        testFunc(QmUSBMode::Ask, QmUSBMode::MassStorage);
    }
    void testMassUndefined() {
        printf("This test will ask you to plug the usb cable in and out.\n");
        printf("This means that you will need to run this via serial cable. :)\n");
        printf("The test starts in 10 seconds...\n");
        QTest::qWait(10*1000);
        testFunc(QmUSBMode::MassStorage, QmUSBMode::Undefined);
    }
    void testOviUndefined() {
        printf("This test will ask you to plug the usb cable in and out.\n");
        printf("This means that you will need to run this via serial cable. :)\n");
        printf("The test starts in 10 seconds...\n");
        QTest::qWait(10*1000);
        testFunc(QmUSBMode::OviSuite, QmUSBMode::Undefined);
    }

    void testAskSDK() {
        printf("This test will ask you to plug the usb cable in and out.\n");
        printf("This means that you will need to run this via serial cable. :)\n");
        printf("The test starts in 10 seconds...\n");
        QTest::qWait(10*1000);
        testFunc(QmUSBMode::Ask, QmUSBMode::SDK);
    }

    void testSDKUndefined() {
        printf("This test will ask you to plug the usb cable in and out.\n");
        printf("This means that you will need to run this via serial cable. :)\n");
        printf("The test starts in 10 seconds...\n");
        QTest::qWait(10*1000);
        testFunc(QmUSBMode::SDK, QmUSBMode::Undefined);
    }

    void testMountOptions() {
        printf("Please unplug the USB cable. Waiting 60 seconds for the /home/user/MyDocs mount to change to writable...\n");
        QTest::qWait(60*1000);
        QmUSBMode::MountOptionFlags mountOptions = qmmode->mountStatus(QmUSBMode::DocumentDirectoryMount);
        verifyMountStatus(true);
    }

    void cleanupTestCase() {
        delete qmmode;
    }
};

QTEST_MAIN(TestClass)
#include "manual_usbmode.moc"
