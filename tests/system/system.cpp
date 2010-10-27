/**
 * @file system.cpp
 * @brief QmSystemState tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Tuomo Tanskanen <ext-tuomo.1.tanskanen@nokia.com>

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
#include <QTest>
#include <QDebug>
#include <QFile>

#include <qmsystemstate.h>

using namespace MeeGo;

class SignalDump : public QObject {
    Q_OBJECT

public:
    SignalDump(QObject *parent = NULL) : QObject(parent) {}

public slots:
    void systemStateChanged(MeeGo::QmSystemState::StateIndication){}
};


class TestClass : public QObject
{
    Q_OBJECT

private:
    MeeGo::QmSystemState *systemstate;
    SignalDump signalDump;

    void runStateFunc(QString stateStr, QmSystemState::RunState state) {
        QString filename = "/var/lib/dsme/saved_state";
        QFile::remove(filename);
        if (stateStr.isEmpty()) {
            QVERIFY2(systemstate->getRunState() == state, "Nonexistent file");
        } else {
            QFile file(filename);
            QVERIFY(file.open(QIODevice::ReadWrite));
            QTextStream out(&file);
            out << stateStr;
            file.close();

            QVERIFY2(systemstate->getRunState() == state, stateStr.toAscii().data());
        }

    }

    void bootReasonFunc(QString reasonStr, QmSystemState::BootReason reason) {
        QString filename = "/proc/bootreason";
        QFile::remove(filename);
        if (reasonStr.isEmpty()) {
            QVERIFY2(systemstate->getBootReason() == reason, "Nonexistent file");
        } else {
            QFile file(filename);
            QVERIFY(file.open(QIODevice::ReadWrite));
            QTextStream out(&file);
            out << reasonStr;
            file.close();

            QVERIFY2(systemstate->getBootReason() == reason, reasonStr.toAscii().data());
        }
    }

private slots:
    void initTestCase() {
        systemstate = new MeeGo::QmSystemState();
        QVERIFY(systemstate);
        QVERIFY(connect(systemstate, SIGNAL(systemStateChanged(MeeGo::QmSystemState::StateIndication)),
                        &signalDump, SLOT(systemStateChanged(MeeGo::QmSystemState::StateIndication))));
    }

    void testSetReboot() {
        bool result = systemstate->set(MeeGo::QmSystemState::Reboot);
        qDebug() << result ;
        QVERIFY(result == true);
    }

    void testSetShutdown() {
        bool result = systemstate->set(MeeGo::QmSystemState::ShuttingDown);
        qDebug() << result ;
        QVERIFY(result == true);
    }

    void testSetPowerup() {
        bool result = systemstate->set(MeeGo::QmSystemState::Powerup);
        qDebug() << result ;
        QVERIFY(result == true);
    }

    void testGetRunstate() {
        runStateFunc("LOCAL", QmSystemState::RunState_Local);
        runStateFunc("ACT_DEAD", QmSystemState::RunState_ActDead);
        runStateFunc("TEST", QmSystemState::RunState_Test);
        runStateFunc("MALF", QmSystemState::RunState_Malf);
        runStateFunc("FLASH", QmSystemState::RunState_Flash);
        runStateFunc("SHUTDOWN", QmSystemState::RunState_Shutdown);
        runStateFunc("garbage", QmSystemState::RunState_Unknown);
        runStateFunc("", QmSystemState::RunState_Unknown);
    }

    void testGetBootreason() {
        bootReasonFunc("swdg_to", QmSystemState::BootReason_SwdgTimeout);
        bootReasonFunc("sec_vio", QmSystemState::BootReason_SecViolation);
        bootReasonFunc("32wd_to", QmSystemState::BootReason_Wdg32kTimeout);
        bootReasonFunc("por", QmSystemState::BootReason_PowerOnReset);
        bootReasonFunc("pwr_key", QmSystemState::BootReason_PowerKey);
        bootReasonFunc("mbus", QmSystemState::BootReason_MBus);
        bootReasonFunc("charger", QmSystemState::BootReason_Charger);
        bootReasonFunc("usb", QmSystemState::BootReason_Usb);
        bootReasonFunc("sw_rst", QmSystemState::BootReason_SWReset);
        bootReasonFunc("rtc_alarm", QmSystemState::BootReason_RTCAlarm);
        bootReasonFunc("garbage", QmSystemState::BootReason_Unknown);
        bootReasonFunc("", QmSystemState::BootReason_Unknown);
    }

    void cleanupTestCase() {
        delete systemstate;
    }
};

QTEST_MAIN(TestClass)
#include "system.moc"
