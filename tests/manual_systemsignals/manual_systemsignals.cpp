/* copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

#include <qmsystem2/qmsystemstate.h>
#include <qmsystem2/qmsysteminformation.h>

using namespace MeeGo;


/*
 * These tests are removed from tests.xml as these tests stops the testrunner script
 */


class tst_systemsignals : public QObject
{
    Q_OBJECT

public:
    tst_systemsignals();
    virtual ~tst_systemsignals();
     MeeGo::QmSystemState::StateIndication state1;


public slots:
    void receiveStateChange(MeeGo::QmSystemState::StateIndication newstate){
        qDebug()<<"New system state::"<<newstate;
        state1 = newstate;
    }

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void tst_rebootDeniedUSBsignal();
    void tst_shutdownDeniedUSBsignal();

private:
    QString bus_name;
    QmSystemState* systemstate;
    MeeGo::QmSystemState::StateIndication state;
};

tst_systemsignals::tst_systemsignals()
{
}

tst_systemsignals:: ~tst_systemsignals()
{
}

void tst_systemsignals::initTestCase()
{
    systemstate = new MeeGo::QmSystemState();
    QVERIFY(systemstate);
}

void tst_systemsignals::cleanupTestCase()
{
    delete systemstate;
    (void)systemstate;
}

void tst_systemsignals::tst_rebootDeniedUSBsignal()
{
    qDebug()<<"\nPre condition is device should be in Mass Storage mode and need to run this test in device terminal\n";

    qRegisterMetaType<MeeGo::QmSystemState::StateIndication>("MeeGo::QmSystemState::StateIndication");
    QObject::connect(systemstate, SIGNAL(systemStateChanged(MeeGo::QmSystemState::StateIndication)), this, SLOT(receiveStateChange(MeeGo::QmSystemState::StateIndication)));
    QSignalSpy spy(systemstate,SIGNAL(systemStateChanged(MeeGo::QmSystemState::StateIndication)));
    bool  rebootstateresult = systemstate->set(QmSystemState::Reboot);

    QVERIFY2(rebootstateresult,"failed in setting reboot state");

    QTest::qWait(1000);

     QVERIFY(spy.count()>0);
     QVERIFY2(state1==MeeGo::QmSystemState::RebootDeniedUSB,"reboot denied failed");

}
void tst_systemsignals::tst_shutdownDeniedUSBsignal()
{
    qDebug()<<"\nPre condition is device should be in Mass Storage mode and need to run this test in device terminal\n";

    qRegisterMetaType<MeeGo::QmSystemState::StateIndication>("MeeGo::QmSystemState::StateIndication");
    QObject::connect(systemstate, SIGNAL(systemStateChanged(MeeGo::QmSystemState::StateIndication)), this, SLOT(receiveStateChange(MeeGo::QmSystemState::StateIndication)));
    QSignalSpy spy(systemstate,SIGNAL(systemStateChanged(MeeGo::QmSystemState::StateIndication)));
    bool  shutdownstateresult = systemstate->set(QmSystemState::ShuttingDown);

    QVERIFY2(shutdownstateresult,"failed in setting shutdown state");

    QTest::qWait(1000);

    QVERIFY(spy.count()>0);
    QVERIFY2(state1==MeeGo::QmSystemState::ShutdownDeniedUSB,"shutdown denied failed");

}



QTEST_MAIN(tst_systemsignals);

#include "manual_systemsignals.moc"
