
#include <QObject>
#include <qmusbmode.h>
#include <QTest>
#include <QStack>
#include <QDebug>
#include <iostream>

using namespace MeeGo;

typedef QStack<QmUSBMode::Mode> ModeStack;

class TestClass : public QObject
{
    Q_OBJECT

public slots:
    void modeChanged(MeeGo::QmUSBMode::Mode mode) {
        qDebug() << "Received a modeChanged signal: " << mode2str(mode);
        currentMode = mode;
        signalReceived = true;
        modeStack.push(mode);
    }

private:
    QmUSBMode *qmmode;
    QmUSBMode::Mode currentMode;
    bool signalReceived;
    ModeStack modeStack;

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
        case QmUSBMode::Undefined:
            return "Undefined";
        default:
            return "An unknown mode!";
        }
    }

    void testFunc(QmUSBMode::Mode defaultMode, QmUSBMode::Mode mode) {
        printf("\n\nTesting with defaultMode %s and mode %s\n\n", mode2str(defaultMode).toAscii().data(), mode2str(mode).toAscii().data());

        printf("\n\nPlease remove the usb cable.\n");
        printf("You have 10 seconds...\n\n");
        QTest::qWait(11*1000);
        //QCOMPARE(currentMode, QmUSBMode::Disconnected);
        QVERIFY(currentMode == QmUSBMode::Disconnected ||
                currentMode == QmUSBMode::Undefined);

        signalReceived = false;
        currentMode = QmUSBMode::Undefined;
        QVERIFY(qmmode->setDefaultMode(defaultMode));
        QCOMPARE(qmmode->getDefaultMode(), defaultMode);

        printf("\n\nPlease plug in the usb cable.\n");
        printf("You have 10 seconds...\n\n");
        for (int i = 0; i < 11; i++) {
            QTest::qWait(1000);
            if (currentMode == defaultMode) {
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
            QVERIFY(qmmode->setMode(mode));
            QTest::qWait(1000);
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

    void cleanupTestCase() {
        delete qmmode;
    }
};

QTEST_MAIN(TestClass)
#include "manual_usbmode.moc"
