/**
 * @file heartbeat.cpp
 * @brief QmHeartbeat tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>

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
#include <qmheartbeat.h>
#include <QTest>
#include <QTimer>
#include <QDebug>


static int got_signal = 0;

class SignalDump : public QObject {
    Q_OBJECT

public:
    SignalDump(QObject *parent = NULL) : QObject(parent) {  }

public slots:
    void wakeUp(QTime time){ qDebug() << "Woke up, consumed" << time; got_signal = 1; }
};


class TestClass : public QObject
{
    Q_OBJECT

private:
    MeeGo::QmHeartbeat *heartbeat;
    SignalDump signalDump;
    
private slots:
    void initTestCase() {
        heartbeat = new MeeGo::QmHeartbeat();
        QVERIFY(heartbeat);
    }

    void testOpen(){
        bool result = heartbeat->open(MeeGo::QmHeartbeat::SignalNeeded);
        QVERIFY(result == true);
    }

    void testClose(){
        heartbeat->close();
    }

    void testConnectSignals() {
        bool result = heartbeat->open(MeeGo::QmHeartbeat::SignalNeeded);
        QVERIFY(connect(heartbeat, SIGNAL(wakeUp(QTime)),
                &signalDump, SLOT(wakeUp(QTime))));
        QVERIFY(result == true);
    }

    void testGetFD(){
        int result = heartbeat->getFD();
        QVERIFY(result != -1);
    }

    void testWait() {

        qDebug() << "Test syncronous wait...";
        int sleeptime = 4;
        qDebug() << "Wait " <<  sleeptime << " seconds" ;
        QTime result = heartbeat->wait(0, sleeptime , MeeGo::QmHeartbeat::WaitHeartbeat);
        QVERIFY(result.second() >= sleeptime &&  result.second() <= sleeptime + 1);
        qDebug() << "Slept " << result.second() << "seconds" ;
        QVERIFY(got_signal == 0);


        qDebug() << "Test async wait, cancel immediately";
        sleeptime = 7;
        result = heartbeat->wait(0, sleeptime, MeeGo::QmHeartbeat::DoNotWaitHeartbeat);
        QVERIFY(result.second() >= 0 &&  result.second() <= 1);
        heartbeat->IWokeUp();
        QTest::qWait(1000);
        QVERIFY(got_signal == 0);

        qDebug() << "Test async wait, assume that signal was got";
        sleeptime = 7;
        result = heartbeat->wait(0, sleeptime, MeeGo::QmHeartbeat::DoNotWaitHeartbeat);
        QVERIFY(result.second() >= 0 &&  result.second() <= 1);
        QVERIFY(got_signal == 0);
        qDebug() << "Slept " << result.second() << "seconds" ;
        sleeptime = 8;
        qDebug() << "Sleep " <<  sleeptime << " seconds" ;
        while (sleeptime > 0 && !got_signal) {
           QTest::qWait(1000);
           sleeptime--;
           qDebug("zzz");
        }
        heartbeat->IWokeUp();

        QVERIFY(got_signal == 1);

    }

    void testNoSignalNeeded() {
        heartbeat->close();
        QVERIFY(heartbeat->open(MeeGo::QmHeartbeat::NoSignalNeeded));
        got_signal=0;
        int sleep_s = 4;

        QTime result = heartbeat->wait(0, sleep_s, MeeGo::QmHeartbeat::WaitHeartbeat);
        QVERIFY(result.second() >= sleep_s && result.second() <= sleep_s+1);
        QVERIFY(got_signal == 0);
        heartbeat->IWokeUp();

        sleep_s=7;
        result = heartbeat->wait(0, 7, MeeGo::QmHeartbeat::DoNotWaitHeartbeat);
        QVERIFY(result.second() >= 0 &&  result.second() <= 1);
        QTest::qWait(8000);
        heartbeat->IWokeUp();
        QVERIFY(got_signal == 0);
    }

   void cleanupTestCase() {
        delete heartbeat;
    }
};

QTEST_MAIN(TestClass)
#include "heartbeat.moc"
