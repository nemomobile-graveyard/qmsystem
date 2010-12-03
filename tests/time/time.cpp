/**
 * @file time.cpp
 * @brief QmTime tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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
#include <qmtime.h>
#include <QTest>
#include <QDebug>

#include "zone.inc"

using namespace MeeGo;

class SignalDump : public QObject {
    Q_OBJECT

public:
    SignalDump(QObject *parent = NULL) : QObject(parent), signalReceived(false) {}

    bool signalReceived;
    QmTimeWhatChanged whatChanged;

public slots:
    void timeOrSettingsChanged(MeeGo::QmTimeWhatChanged whatChanged) {
        qDebug() << "timeOrSettingsChanged:"<<whatChanged;
        signalReceived = true;
        this->whatChanged = whatChanged;
    }
};

void waitTwice(int ms) {
    QTest::qWait(ms);
    QTest::qWait(ms);
}

struct testTZStruct {
    QString tz;
    QString tzname;
    int secsTo;
    QString zone; // the main Olson name of the time zone (if 'tz' is just an alias)
};

class TestClass : public QObject
{
    Q_OBJECT

private:
    MeeGo::QmTime *time;
    SignalDump signalDump;
    
    QDateTime oldTime;
    QString oldTz;
    QmTime::TimeFormat oldTimeFormat;

private slots:
    void initTestCase() {
        time = new MeeGo::QmTime();
        QVERIFY(time);
        oldTime = QDateTime::currentDateTime();
        QVERIFY(time->getTimezone(oldTz));
        oldTimeFormat = time->getTimeFormat();
        QVERIFY(connect(time, SIGNAL(timeOrSettingsChanged(MeeGo::QmTimeWhatChanged)), &signalDump, SLOT(timeOrSettingsChanged(MeeGo::QmTimeWhatChanged))));
    }

    void testGetNetTime() {
        bool ret = true;
        // TODO : to simulate in case that autosync is not
        // enabled and a network time change indication has received)
        //ret = time->getNetTime(t, tz);
        QVERIFY(ret);
    }

    void testSetTime() {
        QDateTime dateTime = QDateTime::fromString("M1d1y9800:01:02",
                                             "'M'M'd'd'y'yyhh:mm:ss");
        signalDump.signalReceived = false;
        bool ret = time->setTime(dateTime);
        QVERIFY(ret);
        waitTwice(500);
        QVERIFY(signalDump.signalReceived);
        QVERIFY(signalDump.whatChanged == QmTimeTimeChanged);

        QVERIFY(dateTime.secsTo(QDateTime::currentDateTime()) <= 3);
    }


    void testSetGetTimezone() {
        // First init to "dummy"
        QVERIFY(time->setTimezone("Asia/Thimphu"));
        waitTwice(500);

        // Then change to Helsinki, so we get the change
        signalDump.signalReceived = false;
        QString tz = "Europe/Helsinki";
        QVERIFY(time->setTimezone(tz));
        waitTwice(500);
        QVERIFY(signalDump.signalReceived);
        QVERIFY(signalDump.whatChanged == QmTimeTimeChanged);

        QString s;
        QVERIFY(time->getTimezone(s));
        QCOMPARE(s, tz);

        QDateTime dateTime = QDateTime::fromString("M1d1y9800:01:02",
                                             "'M'M'd'd'y'yyhh:mm:ss");
        signalDump.signalReceived = false;
        bool ret = time->setTime(dateTime);
        waitTwice(500);
        QVERIFY(signalDump.signalReceived);
        QVERIFY(signalDump.whatChanged == QmTimeTimeChanged);

        QVERIFY(time->getTZName(s));
        QCOMPARE(s, QString("EET"));
    }

    void testSetGetTimeFormat() {

        MeeGo::QmTime::TimeFormat tf = time->getTimeFormat();
        QVERIFY((tf == QmTime::format24h) || (tf == QmTime::format12h));
        qDebug() <<"Current format is "<<tf;

        //qDebug() <<"Wait for gconftool to change the time format key";
        //waitTwice(10000);
        signalDump.signalReceived = false;
        if (tf == QmTime::format12h) {
            QVERIFY(time->setTimeFormat(QmTime::format24h));
        } else {
            QVERIFY(time->setTimeFormat(QmTime::format12h));
        }

        qDebug() <<"Format is changed to "<<time->getTimeFormat();
        waitTwice(600);
        QVERIFY(signalDump.signalReceived);
        QVERIFY(signalDump.whatChanged == QmTimeOnlySettingsChanged);
        QVERIFY(time->getTimeFormat() != tf);
    }

    void testGetUTCOffset() {

        QVERIFY(time->setTimezone("UTC"));
        QDateTime dateTime(QDate(2000, 1, 1));
        QVERIFY(time->setTime(dateTime));

        QCOMPARE(time->getUTCOffset(":Europe/Helsinki"), 2*60*60);
        QCOMPARE(time->getUTCOffset(":US/Central"), -6*60*60);
        QCOMPARE(time->getUTCOffset(":Australia/Melbourne"), 11*60*60);
        QCOMPARE(time->getUTCOffset(":America/Sao_Paulo"), -2*60*60);

        dateTime.setDate(QDate(2000, 6, 6));
        QVERIFY(time->setTime(dateTime));

        QCOMPARE(time->getUTCOffset(":Europe/Helsinki"), 3*60*60);
        QCOMPARE(time->getUTCOffset(":US/Central"), -5*60*60);
        QCOMPARE(time->getUTCOffset(":Australia/Melbourne"), 10*60*60);
        QCOMPARE(time->getUTCOffset(":America/Sao_Paulo"), -3*60*60);
    }

    void testGetDSTUsage() {

        QCOMPARE(time->getDSTUsage(QDateTime(QDate(2000, 1, 1)), ":Europe/Helsinki"), 0);
        QCOMPARE(time->getDSTUsage(QDateTime(QDate(2000, 7, 7)), ":Europe/Helsinki"), 1);
        QCOMPARE(time->getDSTUsage(QDateTime(QDate(2000, 1, 1)), ":Australia/Melbourne"), 1);
        QCOMPARE(time->getDSTUsage(QDateTime(QDate(2000, 7, 7)), ":Australia/Melbourne"), 0);
        QCOMPARE(time->getDSTUsage(QDateTime(QDate(2000, 1, 1)), ":America/Sao_Paulo"), 1);
        QCOMPARE(time->getDSTUsage(QDateTime(QDate(2000, 7, 7)), ":America/Sao_Paulo"), 0);
    }

    void testIsOperatorTimeAccessible() {
        int ret = time->isOperatorTimeAccessible();
        QVERIFY(ret != -1);
    }

    /** The following tests are from old dms-tests.
     *  TODO:
     *
     *  test 120050 does not make sense, as there is no time_mktime() in QmTime?
     *
     *  test 120080 is not probably applicable, because formatting
     *  should be done with QDateTime? Why QmTime has TimeFormats?
     *
     *  test 120090 does not make sense, as there is no time_get_synced() in QmTime?
     */
    void testSetTime120010() {
        QDateTime current = QDateTime::currentDateTime();
        QVERIFY(time->setTime(current.addSecs(5)));
        //waitTwice(500);
        QDateTime current2 = QDateTime::currentDateTime();
        int secsTo = current.secsTo(current2);
        QString str = QString("Time difference: %1").arg(secsTo);
        QVERIFY2(secsTo >= 4, str.toAscii().data());
        QVERIFY2(secsTo <= 5, str.toAscii().data());
    }


    // TODO: GMT* timezones do not work with libtimed0?
    void testSetTimezone120020() {
        struct testTZStruct tzs[] = { //{"GMT-2GMT-3,0,365", "GMT", 3*60*60},
                                      //{"GMT+8", "GMT", -8*60*60},
                                      //{"GMT-10:30", "GMT", 10*60*60 + 30*60},
                                      {"Africa/Algiers", "CET", 60*60, ""},
                                      {"Africa/Cairo", "EET", 2*60*60, ""},
                                      {"Africa/Casablanca", "WET", 0, ""},
                                      {"Asia/Shanghai", "CST", 8*60*60, ""},
                                      {"Asia/Tokyo", "JST", 9*60*60, ""},
                                      {"Australia/Melbourne", "EST", 11*60*60, ""}, // DST on southern summer
                                      {"Canada/Central", "CST", -6*60*60, "America/Winnipeg"},
                                      {"Europe/Helsinki", "EET", 2*60*60, ""},
                                      {"Europe/London", "GMT", 0, ""},
                                      {"Europe/Moscow", "MSK", 3*60*60, ""},
                                      {"Europe/Paris", "CET", 1*60*60, ""},
                                      {"Pacific/Honolulu", "HST", -10*60*60, ""},
                                      {"US/Alaska", "AKST", -9*60*60, "America/Anchorage"},
                                      {"US/Central", "CST", -6*60*60, "America/Chicago"},
                                      {"US/Mountain", "MST", -7*60*60, "America/Denver"},
                                      {"US/Pacific", "PST", -8*60*60, "America/Los_Angeles"}};

        for (unsigned i=0; i < sizeof(tzs)/sizeof(*tzs); i++) {
            QVERIFY(time->setTimezone("UTC"));
            QDateTime dateTime(QDate(2000, 1, 1));
            QVERIFY(time->setTime(dateTime));

            QVERIFY2(time->setTimezone(tzs[i].tz), tzs[i].tz.toAscii().data());

            QString tz2;
            QVERIFY2(time->getTimezone(tz2), tzs[i].tz.toAscii().data());
            QCOMPARE(tzs[i].zone.isEmpty() ? tzs[i].tz : tzs[i].zone, tz2);

            QVERIFY2(time->getTZName(tz2), tzs[1].tz.toAscii().data());
            QCOMPARE(tzs[i].tzname, tz2);

            int diff = dateTime.secsTo(QDateTime::currentDateTime());
            qDebug()<<diff<<tzs[i].secsTo<<i;
            QVERIFY(diff*tzs[i].secsTo >= 0);
            QVERIFY2(qAbs(diff) <= qAbs(tzs[i].secsTo) + 5, tzs[i].tz.toAscii());
        }
    }

    void testSetWrongTimezone120021() {
        QVERIFY(!time->setTimezone("MT-8"));
        QVERIFY(!time->setTimezone("CC"));
    }

    void testMakeAndCheckTime120030() {


        QVERIFY(time->setTimezone("Europe/Helsinki"));
        waitTwice(500);

        /* Set time to 1.7.2008 14:00 and DST on*/
        struct tm tm;
        tm.tm_sec = 0;          /* seconds [0,61] */
        tm.tm_min = 0;          /* minutes [0,59] */
        tm.tm_hour = 14;        /* hour [0,23] */
        tm.tm_mday = 1;         /* day of month [1,31] */
        tm.tm_mon = 6;          /* month of year [0,11] */
        tm.tm_year = 108;       /* years since 1900 */
        tm.tm_wday = 2;         /* day of week [0,6] (Sunday = 0) */
        tm.tm_isdst = 1;        /* daylight savings flag */
        QDateTime dateTime = QDateTime::fromTime_t(mktime(&tm));
        QVERIFY(time->setTime(dateTime));

        QDateTime utc = QDateTime::currentDateTime().toUTC();
        QCOMPARE(utc.time().hour(), 11);

        QDateTime current = QDateTime::currentDateTime();
        QVERIFY(dateTime.secsTo(current) <= 2);
        QVERIFY(dateTime.secsTo(current) >= -2);

        QString tz;
        QVERIFY(time->getTimezone(tz));
        QCOMPARE(time->getDSTUsage(current, tz), 1);
    }

    void testCheckRemoteTime120040() {
        QString timezone = ":Europe/Helsinki";
        QVERIFY(time->setTimezone(timezone));
        /* Set time to 1.7.2008 14:00 and DST on*/
        struct tm tm;
        tm.tm_sec = 0;          /* seconds [0,61] */
        tm.tm_min = 0;          /* minutes [0,59] */
        tm.tm_hour = 14;        /* hour [0,23] */
        tm.tm_mday = 1;         /* day of month [1,31] */
        tm.tm_mon = 6;          /* month of year [0,11] */
        tm.tm_year = 108;       /* years since 1900 */
        tm.tm_wday = 2;         /* day of week [0,6] (Sunday = 0) */
        tm.tm_isdst = 1;        /* daylight savings flag */
        QDateTime dateTime = QDateTime::fromTime_t(mktime(&tm));
        QVERIFY(time->setTime(dateTime));

        QDateTime remoteTime;
        QVERIFY(time->getRemoteTime(QDateTime::currentDateTime(), ":Europe/Paris", remoteTime));
        QCOMPARE(remoteTime.time().hour(), 13);

        QVERIFY(time->getRemoteTime(QDateTime::currentDateTime(), ":Europe/London", remoteTime));
        QCOMPARE(remoteTime.time().hour(), 12);

        QDateTime currentDateTime = QDateTime::currentDateTime();
        QVERIFY(time->getRemoteTime(currentDateTime.addSecs(60*60), "Europe/London", remoteTime));
        QCOMPARE(remoteTime.time().hour(), 13);

        QString tz;
        QVERIFY(time->getTimezone(tz));
        QCOMPARE(tz, timezone);
        QCOMPARE(time->getDSTUsage(QDateTime::currentDateTime(), tz), 1);
        QCOMPARE(time->getDSTUsage(QDateTime(QDate(2000, 1, 1)), tz), 0);
    }

    void testGetTimeDiff120051() {
        time->setTimezone(":Europe/Helsinki");
        int diff = time->getTimeDiff(QDateTime::currentDateTime(), ":Europe/Helsinki", ":Europe/London");
        QCOMPARE(diff, 7200);
    }

    void testGetTimeDiffDST120052() {
        struct tm tm;
        /* 10.3.2009 14:00 and DST on */
        tm.tm_sec = 0;          /* seconds [0,61] */
        tm.tm_min = 0;          /* minutes [0,59] */
        tm.tm_hour = 14;        /* hour [0,23] */
        tm.tm_mday = 10;        /* day of month [1,31] */
        tm.tm_mon = 2;          /* month of year [0,11] */
        tm.tm_year = 109;       /* years since 1900 */
        tm.tm_wday = 2;         /* day of week [0,6] (Sunday = 0) */
        tm.tm_isdst = 1;        /* daylight savings flag */
        QDateTime t = QDateTime::fromTime_t(mktime(&tm));

        int diff = time->getTimeDiff(t, ":Europe/Helsinki", ":GMT");
        QCOMPARE(diff, 7200);

        t = t.addDays(30);
        diff = time->getTimeDiff(t, ":Europe/Helsinki", ":GMT");
        QCOMPARE(diff, 10800);
    }

    void testOperatorTime120070() {
        int ret = time->isOperatorTimeAccessible();
        QVERIFY(ret == 0 || ret == 1);
    }

    // There is no libtime time_format_time() in QmTime.
    void testTimeFormat120080() {
        QVERIFY(time->setTimeFormat(QmTime::format12h));
        QCOMPARE(time->getTimeFormat(), QmTime::format12h);
        QVERIFY(time->setTimeFormat(QmTime::format24h));
        QCOMPARE(time->getTimeFormat(), QmTime::format24h);
        QVERIFY(time->setTimeFormat(QmTime::format12h));
        QCOMPARE(time->getTimeFormat(), QmTime::format12h);
    }

    // TESTCASES FROM OLD clocktest.c

    // TODO: MT is not in /usr/share/zoneinfo
    // TODO: timezone EST+5EDT,M4.1.0/2,M10.5.0/2 is not supported?
    void test_clocktest_start() {
        QVERIFY(!time->setTimezone(""));
        QVERIFY(!time->setTimezone("1"));
        QVERIFY(!time->setTimezone("AA"));
        QVERIFY(!time->setTimezone("BB-"));
        QVERIFY(!time->setTimezone("MT-8"));

        QCOMPARE(time->getTimeDiff(QDateTime::currentDateTime(), ":Europe/Helsinki", ":Europe/London"), 7200);
        /*
        QCOMPARE(time->getTimeDiff(QDateTime::currentDateTime(), "GMT-2", "MT+2"), 0);
        QCOMPARE(time->getTimeDiff(QDateTime::currentDateTime(), "GMT+3", "MT+3"), -6*3600);
        */

        QVERIFY(time->setTime(QDateTime::fromTime_t(1221472803)));

        /*
        QString setTimezone = "EST+5EDT,M4.1.0/2,M10.5.0/2";
        QVERIFY(time->setTimezone(setTimezone));
        QString gotTimezone;
        QVERIFY(time->getTimezone(gotTimezone));
        QCOMPARE(setTimezone, gotTimezone);
        */
    }


    // TODO: the loop test will fail without a//waitTwice(500) call in the clocktest_loop.
    void test_clocktest_loop() {
        for (int i=0; i < 10; i++) {
            clocktest_loop();
        }
    }

    void clocktest_loop() {

        QDateTime currentDateTime = QDateTime::currentDateTime();
        QVERIFY(time->setTime(currentDateTime.addSecs(2)));

        QVERIFY(currentDateTime.secsTo(QDateTime::currentDateTime()) < 6);

        currentDateTime = QDateTime::currentDateTime();
        QString timezone;
        QVERIFY(time->getTimezone(timezone));


        for (int i = 0; i < zonecnt; i++) {
            QDateTime remoteTime;
            QVERIFY(time->getRemoteTime(currentDateTime, zone[i], remoteTime));
        }

        QVERIFY(time->isOperatorTimeAccessible() == 0 || time->isOperatorTimeAccessible() == 1);


        currentDateTime = QDateTime::currentDateTime();
        qDebug() << "currentDateTime: " << currentDateTime;
        QVERIFY(currentDateTime.secsTo(QDateTime::currentDateTime()) < 6);

        qDebug() << "timezone: " << zone[0];
        QVERIFY2(time->setTimezone(zone[0]), zone[0]);
        // TODO: Without wait here the test will fail.
        //waitTwice(500);
        currentDateTime = QDateTime::currentDateTime();
        qDebug() << "currentDateTime: " << currentDateTime;
        QVERIFY(currentDateTime.secsTo(QDateTime::currentDateTime()) < 6);

        currentDateTime = QDateTime::currentDateTime();
        qDebug() << "currentDateTime: " << currentDateTime;
        QVERIFY(time->setTime(currentDateTime.addSecs(3)));
        int secsTo = currentDateTime.secsTo(QDateTime::currentDateTime());
        qDebug() << "secsTo: " << secsTo;
        QVERIFY(secsTo < 6 && secsTo >= 2);
        QVERIFY(time->setTime(currentDateTime.addSecs(2)));
        QVERIFY(currentDateTime.secsTo(QDateTime::currentDateTime()) < 10);

        for (int i = 0; i < zonecnt; i++) {
            QVERIFY(time->setTimezone(zone[i]));
        }
    }

    void cleanupTestCase() {
        QVERIFY(time->setTime(oldTime));
        QVERIFY(time->setTimezone(oldTz));
        //QVERIFY(time->setTimeFormat(oldTimeFormat));
        delete time;
    }
};

QTEST_MAIN(TestClass)
#include "time.moc"
