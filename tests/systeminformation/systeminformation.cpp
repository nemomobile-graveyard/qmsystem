/**
 * @file systeminformation.cpp
 * @brief System Information tests.

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Matias Muhonen <ext-matias.muhonen@nokia.com>

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
#include <QFile>

#include <qmsysteminformation.h>

using namespace MeeGo;

class TestClass : public QObject
{
    Q_OBJECT

private slots:
    void testSystemInformation() {
        MeeGo::QmSystemInformation systemInformation;

        QString v1 = systemInformation.valueForKey("/device/sw-release-ver");
        QString v2 = systemInformation.valueForKey("/device/sw-release-ver");
        QString v3 = systemInformation.valueForKey("/device/sw-release-ver");

        QVERIFY(v1.length() > 5);

        /* SysInfo data is static, so we must get the same value every time we query a key */
        QVERIFY(v1.compare(v2) == 0);
        QVERIFY(v1.compare(v3) == 0);
    }

    void testOperatingSystemPr10() {
        MeeGo::QmSystemInformation::OperatingSystemVersion v =
             MeeGo::QmSystemInformation::operatingSystemVersion("DFL61_HARMATTAN_10.2011.34-1_PR_LEGACY_001_ARM.bin");

        QVERIFY(v == MeeGo::QmSystemInformation::OSVersion_Harmattan_1_0);

        MeeGo::QmSystemInformation::OperatingSystemVersion v2 =
             MeeGo::QmSystemInformation::operatingSystemVersion("DFL61_HARMATTAN_1.2011.34-1_PR_LEGACY_001_ARM.bin");

        QVERIFY(v2 == MeeGo::QmSystemInformation::OSVersion_Harmattan_1_0);
    }

    void testOperatingSystemPr11() {
        MeeGo::QmSystemInformation::OperatingSystemVersion v =
             MeeGo::QmSystemInformation::operatingSystemVersion("DFL61_HARMATTAN_20.2011.34-1_PR_LEGACY_001_ARM.bin");

        QVERIFY(v == MeeGo::QmSystemInformation::OSVersion_Harmattan_1_1);

        MeeGo::QmSystemInformation::OperatingSystemVersion v2 =
             MeeGo::QmSystemInformation::operatingSystemVersion("DFL61_HARMATTAN_2.2011.34-1_PR_LEGACY_001_ARM.bin");

        QVERIFY(v2 == MeeGo::QmSystemInformation::OSVersion_Harmattan_1_1);
    }

    void testOperatingSystemPr111() {
        MeeGo::QmSystemInformation::OperatingSystemVersion v =
             MeeGo::QmSystemInformation::operatingSystemVersion("DFL61_HARMATTAN_22.2011.34-1_PR_LEGACY_001_ARM.bin");

        QVERIFY(v == MeeGo::QmSystemInformation::OSVersion_Harmattan_1_1_1);
    }

    void testOperatingSystemPr12() {
        MeeGo::QmSystemInformation::OperatingSystemVersion v =
             MeeGo::QmSystemInformation::operatingSystemVersion("DFL61_HARMATTAN_30.2011.34-1_PR_LEGACY_001_ARM.bin");

        QVERIFY(v == MeeGo::QmSystemInformation::OSVersion_Harmattan_1_2);

        MeeGo::QmSystemInformation::OperatingSystemVersion v2 =
             MeeGo::QmSystemInformation::operatingSystemVersion("DFL61_HARMATTAN_3.2011.34-1_PR_LEGACY_001_ARM.bin");

        QVERIFY(v2 == MeeGo::QmSystemInformation::OSVersion_Harmattan_1_2);

       MeeGo::QmSystemInformation::OperatingSystemVersion v3 =
             MeeGo::QmSystemInformation::operatingSystemVersion("DFL61_HARMATTAN_40.2011.34-1_PR_LEGACY_001_ARM.bin");

        QVERIFY(v3 == MeeGo::QmSystemInformation::OSVersion_Harmattan_1_2);
    }

    void testInvalidOperatingSystemVersion() {
        MeeGo::QmSystemInformation::OperatingSystemVersion v =
             MeeGo::QmSystemInformation::operatingSystemVersion("DFL61_HARMATTAN_DIIBADAABA.bin");

        QVERIFY(v == MeeGo::QmSystemInformation::OSVersion_Unknown);
    }

    void testOperatingSystemVersion() {
        MeeGo::QmSystemInformation::OperatingSystemVersion v =
            MeeGo::QmSystemInformation::operatingSystemVersion();

        QVERIFY(v > MeeGo::QmSystemInformation::OSVersion_Unknown);
        QVERIFY(v > MeeGo::QmSystemInformation::OSVersion_Harmattan_1_0);
        QVERIFY(v > MeeGo::QmSystemInformation::OSVersion_Harmattan_1_1);
        QVERIFY(v > MeeGo::QmSystemInformation::OSVersion_Harmattan_1_1_1);
        QVERIFY(v >= MeeGo::QmSystemInformation::OSVersion_Harmattan_1_2);
    }
};

QTEST_MAIN(TestClass)
#include "systeminformation.moc"
