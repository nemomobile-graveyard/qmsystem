/**
 * @file usbmode.cpp
 * @brief QmUSBMode tests

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
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
#include <qmusbmode.h>
#include <QTest>
#include <QDebug>

#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

using namespace MeeGo;

class TestClass : public QObject
{
    Q_OBJECT

public slots:
    void modeChanged(MeeGo::QmUSBMode::Mode mode) {
        qDebug() << "Received a modeChanged signal: " << mode;
        currentMode = mode;
        signalReceived = true;
    }

    void error(const QString &errorCode) {
        qDebug() << "Received a ERROR signal: " << errorCode;
        signalReceived = true;
    }

private:
    QmUSBMode *mode;
    QmUSBMode::Mode currentMode;
    bool signalReceived;

    void setGetDefaultMode(QmUSBMode::Mode newMode) {
        QVERIFY(mode->setDefaultMode(newMode));
        QCOMPARE(newMode, mode->getDefaultMode());
    }

private slots:
    void initTestCase() {
        mode = new QmUSBMode();
        QVERIFY(mode);
        QVERIFY(connect(mode, SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode)),
                        this, SLOT(modeChanged(MeeGo::QmUSBMode::Mode))));
        QVERIFY(connect(mode, SIGNAL(error(const QString)),
                        this, SLOT(error(const QString))));
    }

    void testSetGetDefaultModes() {
        setGetDefaultMode(QmUSBMode::MassStorage);
        setGetDefaultMode(QmUSBMode::ChargingOnly);
        setGetDefaultMode(QmUSBMode::OviSuite);
        setGetDefaultMode(QmUSBMode::Ask);
    }

    void testMountStatus() {
        struct timespec ts = { 0, 0 };
        char *path = 0;
        FILE *f;
        QmUSBMode::MountOptionFlags mountOptions = mode->mountStatus(QmUSBMode::DocumentDirectoryMount);

        if (mountOptions.testFlag(MeeGo::QmUSBMode::ReadWriteMount)) {
            qDebug() << "Got the QmUSBMode::ReadWriteMount flag";
        }

        if (mountOptions.testFlag(MeeGo::QmUSBMode::ReadOnlyMount)) {
            qDebug() << "Got the QmUSBMode::ReadOnlyMount flag";
        }

        QVERIFY(clock_gettime(CLOCK_MONOTONIC, &ts) == 0);
        QVERIFY(asprintf(&path, "/home/user/MyDocs/qmsystem.usbmode-test.%lu.%lu",
                         (unsigned long)ts.tv_sec,
                         (unsigned long)ts.tv_nsec) != -1);

        qDebug() << "Checking read and write access to " << path;

        f = fopen(path, "a+");
        if (f) {
            qDebug() << "Got read and write access to /home/user/MyDocs";

            QVERIFY(mountOptions.testFlag(MeeGo::QmUSBMode::ReadWriteMount));
            fclose(f);
        } else {
            qDebug() << "No read and write access to /home/user/MyDocs: " <<  strerror(errno);

            if (access("/home/user/MyDocs", R_OK) == 0) {
                qDebug() << "Got read access to /home/user/MyDocs";

                QVERIFY(mountOptions.testFlag(MeeGo::QmUSBMode::ReadOnlyMount));
            } else {
                qDebug() << "No read or write access to /home/user/MyDocs";

                QVERIFY(!mountOptions.testFlag(MeeGo::QmUSBMode::ReadWriteMount));
                QVERIFY(!mountOptions.testFlag(MeeGo::QmUSBMode::ReadOnlyMount));
            }
        }

        free(path);
    }

    void cleanupTestCase() {
        delete mode;
    }
};

QTEST_MAIN(TestClass)
#include "usbmode.moc"
