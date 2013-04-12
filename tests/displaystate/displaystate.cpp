/**
 * @file displaystate.cpp
 * @brief QmDisplayState tests

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
#include <qmdisplaystate.h>
#include <QTest>
#include <QDebug>
#include <QList>

static const int WAIT_TIME_MS = 1500;

static QString displayStateToString(MeeGo::QmDisplayState::DisplayState displayState) {
    QString state = "Unknown";

    if (MeeGo::QmDisplayState::Off == displayState) {
        state = "Off";
    } else if (MeeGo::QmDisplayState::Dimmed == displayState) {
        state = "Dimmed";
    } else if (MeeGo::QmDisplayState::On == displayState) {
        state = "On";
    }
    return state;
}

class SignalDump : public QObject {
    Q_OBJECT

public:
    SignalDump(QObject *parent = NULL) : QObject(parent) {}

    MeeGo::QmDisplayState displayState;
    QList<MeeGo::QmDisplayState::DisplayState> receivedStates;

public slots:
    void displayStateChanged(MeeGo::QmDisplayState::DisplayState newState ) {
		receivedStates << newState;
        qDebug() << "Received state changed signal: " << displayStateToString(newState);
        QVERIFY(displayState.get() == newState);
    }
};


class TestClass : public QObject
{
    Q_OBJECT

private:
    MeeGo::QmDisplayState *displaystate;
    SignalDump signalDump;

private slots:
    void initTestCase() {
        qDebug() << "initTestCase called";

        displaystate = new MeeGo::QmDisplayState();
        QVERIFY(displaystate);
        QVERIFY(connect(displaystate, SIGNAL(displayStateChanged(MeeGo::QmDisplayState::DisplayState)),
                &signalDump, SLOT(displayStateChanged(MeeGo::QmDisplayState::DisplayState))));
    }

    void setDisplayState(MeeGo::QmDisplayState::DisplayState displayState) {
        (void)displaystate->set(displayState);

        qDebug() << "setDisplayState: " << displayStateToString(displayState);
        QTest::qWait(WAIT_TIME_MS);
    }

    void testSetStateOff() {
        signalDump.receivedStates.clear();

        setDisplayState(MeeGo::QmDisplayState::On);
        setDisplayState(MeeGo::QmDisplayState::Off);

        QVERIFY(signalDump.receivedStates.contains(MeeGo::QmDisplayState::Off));

        QTest::qWait(WAIT_TIME_MS * 2);
    }

    void testSetStateDimmed() {
        signalDump.receivedStates.clear();

        setDisplayState(MeeGo::QmDisplayState::On);
        setDisplayState(MeeGo::QmDisplayState::Dimmed);

        QVERIFY(signalDump.receivedStates.contains(MeeGo::QmDisplayState::Dimmed));

        QTest::qWait(WAIT_TIME_MS * 2);
    }

    void testSetStateOn() {
        signalDump.receivedStates.clear();

        setDisplayState(MeeGo::QmDisplayState::Off);
        setDisplayState(MeeGo::QmDisplayState::On);

        QVERIFY(signalDump.receivedStates.contains(MeeGo::QmDisplayState::On));

        QTest::qWait(WAIT_TIME_MS * 2);
    }

    void testSetBlankingPause() {
        bool result = displaystate->setBlankingPause();
        QVERIFY(result == true);
    }

    void testGetMaxDisplayBrightnessValue() {
        qDebug() << "DisplayMaxBrightnessValue: " << displaystate->getMaxDisplayBrightnessValue();
    }

    void testDisplayBrightnessValue() {
        int originalDisplayBrightnessValue = displaystate->getDisplayBrightnessValue();

        displaystate->setDisplayBrightnessValue(3);
        int result = displaystate->getDisplayBrightnessValue();
        QVERIFY(result == 3);
        displaystate->setDisplayBrightnessValue(2);
        result = displaystate->getDisplayBrightnessValue();
        QVERIFY(result == 2);

        displaystate->setDisplayBrightnessValue(originalDisplayBrightnessValue);
    }

    void testDisplayBlankTimeout() {
        int originalDisplayBlankTimeout = displaystate->getDisplayBlankTimeout();

        displaystate->setDisplayBlankTimeout(10);
        int result = displaystate->getDisplayBlankTimeout();
        QVERIFY(result == 10);
        displaystate->setDisplayBlankTimeout(15);
        result = displaystate->getDisplayBlankTimeout();
        QVERIFY(result == 15);

        displaystate->setDisplayBlankTimeout(originalDisplayBlankTimeout);
    }

    void testDisplayDimTimeout() {
        int originalDisplayDimTimeout = displaystate->getDisplayDimTimeout();

        displaystate->setDisplayDimTimeout(15);
        int result = displaystate->getDisplayDimTimeout();
        QVERIFY(result == 15);
        displaystate->setDisplayDimTimeout(60);
        result = displaystate->getDisplayDimTimeout();
        QVERIFY(result == 60);

        displaystate->setDisplayDimTimeout(originalDisplayDimTimeout);
    }

    void testBlankingCharging() {
        bool originalBlankingWhenCharging = displaystate->getBlankingWhenCharging();

        displaystate->setBlankingWhenCharging(true);
        bool result = displaystate->getBlankingWhenCharging();
        QVERIFY(result == true);

        displaystate->setBlankingWhenCharging(false);
        result = displaystate->getBlankingWhenCharging();
        QVERIFY(result == false);

        displaystate->setBlankingWhenCharging(originalBlankingWhenCharging);
    }

    void cleanupTestCase() {
        qDebug() << "cleanupTestCase called";

        delete displaystate;
    }
};

QTEST_MAIN(TestClass)
#include "displaystate.moc"
