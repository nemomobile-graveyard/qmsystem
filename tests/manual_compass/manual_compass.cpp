/**
 * @file manual_compass.cpp
 * @brief QmCompass manual tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

   This file is part of SystemSW QtAPI.

   SystemSW QtAPI is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   SystemSW QtAPI is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with SystemSW QtAPI.  If not, see <http://www.gnu.org/licenses/>.
   </p>
 */

#include <QObject>
#include <qmcompass.h>
#include <QTest>
#include <QList>
#include <QTime>
#include <cstdio>
#include <gconf/gconf-client.h>

#define TOLERANCE_AVG 15

using namespace MeeGo;

class TestClass : public QObject
{
    Q_OBJECT

public slots:

    void compassChanged(const MeeGo::QmCompassReading newDegrees) {
        //printf("\tSesnor NO declination : %d\n", newDegrees.degrees);
        if (readLevel) {
            level = newDegrees.level;
            printf("\t Calibration level: %d\n", newDegrees.level);
        } else {

            if (printCompass && lastPrint.elapsed() >= 500) {
                printf("\tcurrent angle from north: %d\n", newDegrees.degrees);
                lastPrint.restart();
            }
            if (readCompass) {
                compassValues.push_back(newDegrees.degrees);
                printf("\tmeasured angle from north:%d\n", newDegrees.degrees);
            } else {
                compassValues.clear();
                compassValues.push_back(newDegrees.degrees);
            }
        }
    }

    void compassDecChanged(const MeeGo::QmCompassReading newDegrees) {
        Q_UNUSED(newDegrees);
        count++;
        //printf("\tSesnor YES declination : %d\n", newDegrees.degrees);

    }

private:

    void calibrate() {
        level = sensor->get().level;
        printf("\tlevel: %d\n", level);
        if (level >= 3) {
            return;
        }
        printf("\n\n");
        printf("First, you need to calibrate the compass. Please rotate the device around with your wrist.\n");
        printf("Calibration level 3 is required...\n");
        readLevel = true;
        while (true) {
            QTest::qWait(1000);
            if (level >= 3) {
                break;
            }
        }
        readLevel = false;
        printf("\nRequired calibration level reached.\n");
    }

    void testFunc(const QString &msg, int wantedDeg) {
        printf("%s", msg.toAscii().data());
        printf("The required angle from north is %d.\n", wantedDeg);
        printf("You have 10 seconds...\n\n");
        compassValues.clear();
        printCompass = true;
        lastPrint.start();
        QTest::qWait(10000);
        printCompass = false;
        printf("Measuring...\n\n");
        readCompass = true;
        QTest::qWait(5000);
        readCompass = false;
        QVERIFY2(compassValues.count() > 0, "Did not receive any compass values!");

        /* Check average */
        int deg;
        int devSum = 0;
        foreach(deg, compassValues) {
            int diffAround = wantedDeg + 360 - deg;
            int diffStraight;
            if (deg >= wantedDeg) {
                diffStraight = deg - wantedDeg;
            } else {
                diffStraight = wantedDeg - deg;
            }
            if (diffAround <= diffStraight) {
                devSum += diffAround;
            } else {
                devSum += diffStraight;
            }
        }
        double devAvg = devSum / compassValues.count();
        QVERIFY(devAvg <= TOLERANCE_AVG);
    }

private slots:
    void initTestCase() {
        count = 0;
        readLevel = false;
        readCompass = false;
        printCompass = false;
        level = 0;
        sensor = new MeeGo::QmCompass();
        sensorDec = new MeeGo::QmCompass();
        QVERIFY(sensor);
        QVERIFY(sensorDec);
        QVERIFY(connect(sensor, SIGNAL(dataAvailable(const MeeGo::QmCompassReading)),
                this, SLOT(compassChanged(const MeeGo::QmCompassReading))));
        QVERIFY(connect(sensorDec, SIGNAL(dataAvailable(const MeeGo::QmCompassReading)),
                this, SLOT(compassDecChanged(const MeeGo::QmCompassReading))));
        QVERIFY2(sensor->requestSession(MeeGo::QmSensor::SessionTypeControl) != MeeGo::QmSensor::SessionTypeNone,
                sensor->lastError().toLocal8Bit());
        QVERIFY2(sensorDec->requestSession(MeeGo::QmSensor::SessionTypeControl) != MeeGo::QmSensor::SessionTypeNone,
                sensorDec->lastError().toLocal8Bit());
        sensor->setStandbyOverride(true);
        sensorDec->setStandbyOverride(true);
        QVERIFY2(sensor->start(), sensor->lastError().toLocal8Bit());        
        QVERIFY2(sensorDec->start(), sensorDec->lastError().toLocal8Bit());
        ep.start();
        g_type_init();
        gcClient = gconf_client_get_default();
    }

    void testDeclination() {

        printf("Test compass declination\n");
        printf("Calibrating sensors. Please rotate....\n\n");
        while(1) {
            if (sensorDec->get().level >= 3 && sensorDec->get().level >= 3)
                printf("SensorDec calibration level is: 3\n\n");
                break;
        }
        sensorDec->setUseDeclination(true);
        QVERIFY(sensorDec->useDeclination());
        printf("SensorDec is using declination\n");
        sensor->setUseDeclination(false);
        QVERIFY(!sensor->useDeclination());
        printf("Sensor is NOT using declination\n");

        gconf_client_set_int(gcClient, "/system/osso/location/settings/magneticvariation", 10, NULL);
        QVERIFY(gconf_client_get_int(gcClient, "/system/osso/location/settings/magneticvariation", NULL) == 10);

        printf("Measuring...Move the device\n\n");
        QTest::qWait(10000);
        printf("SensorDec: %d, Sensor: %d\n", sensorDec->get().degrees, sensor->get().degrees);
        //QVERIFY(sensorDec->get().degrees - sensor->get().degrees == 10);
        printf("SensorDec interval %d\n\n", sensorDec->interval());
        printf("Time elapsed %d\n\n", ep.elapsed());
        printf("Received data reading %d\n\n", count);
    }

    void testManual() {
        calibrate();
        printf("\n\n");
        printf("This test will instruct you to point the device to various compass points.\n");
        printf("You will have 10 seconds to point the device into each direction, and you need to hold it for 5 seconds.\n");
        printf("Get ready! The test will start in 10 seconds...\n");
        QTest::qSleep(10000);
        testFunc(tr("\nPlease point the device to north.\n"), 0);
        testFunc(tr("\nPlease point the device to south.\n"), 180);
        testFunc(tr("\nPlease point the device to northeast.\n"), 45);
        testFunc(tr("\nPlease point the device to southwest.\n"), 225);
        testFunc(tr("\nPlease point the device to east.\n"), 90);
        testFunc(tr("\nplease point the device to west.\n"), 270);
        testFunc(tr("\nPlease point the device to southeast.\n"), 135);
        testFunc(tr("\nPlease point the device to northwest.\n"), 315);
    }


    void cleanupTestCase() {
        QVERIFY2(sensor->stop(), sensor->lastError().toLocal8Bit());
        QVERIFY2(sensorDec->stop(), sensorDec->lastError().toLocal8Bit());
        delete sensor;
        delete sensorDec;
    }

private:
    MeeGo::QmCompass *sensor;
    MeeGo::QmCompass *sensorDec;
    GConfClient *gcClient;

    bool readCompass;
    bool printCompass;
    QTime lastPrint;
    QList<int> compassValues;

    bool readLevel;
    int level;
    int count;
    QTime ep;

};

QTEST_MAIN(TestClass)
#include "manual_compass.moc"
