/**
 * @file cabc.cpp
 * @brief QmCABC tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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
#include <qmcabc.h>
#include <QTest>

class TestClass : public QObject
{
    Q_OBJECT

private:
    MeeGo::QmCABC *cabc;

private slots:
    void initTestCase() {
        cabc = new MeeGo::QmCABC();
        QVERIFY(cabc);
    }

    void testGet() {
        MeeGo::QmCABC::Mode result = cabc->get();
        (void)result;
    }

    void testSet() {
        for(int index=0; index<4; index++ ){
            bool result = cabc->set((MeeGo::QmCABC::Mode)index);
            QVERIFY(result == true);
            QCOMPARE((MeeGo::QmCABC::Mode)index, cabc->get());
        }
    }

    void cleanupTestCase() {
        delete cabc;
    }
};

QTEST_MAIN(TestClass)
#include "cabc.moc"
