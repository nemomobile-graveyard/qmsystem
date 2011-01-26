/*!
 * @file qmtime_p.h
 * @brief Contains QmTimePrivate

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Yang Yang <ext-yang.25.yang@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>

   @scope Private

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
#ifndef QMTIME_P_H
#define QMTIME_P_H

#include <QDebug>
#include <QMutex>
#include <QSocketNotifier>
#include <timed/interface>

#include <fcntl.h>
#include <gconf/gconf-client.h>

#include "qmtime.h"

namespace MeeGo {

    class SaveEnvTz {
    public:
        static QMutex mtx;
        QString envTz;

        SaveEnvTz() {
            mtx.lock();
            char *tmp = getenv("TZ");
            if (tmp) {
                envTz = QString(tmp);
            } else {
                envTz = "";
            }
        }

        ~SaveEnvTz() {
            if (envTz.isEmpty()) {
                unsetenv("TZ");
            } else {
                setenv("TZ", envTz.toAscii().data(), 1);
                tzset();
            }
            mtx.unlock();
        }
    };

    class QmTimePrivate : public QObject
    {
        Q_OBJECT;
        MEEGO_DECLARE_PUBLIC(QmTime);

    public:

        Maemo::Timed::Interface ifc;

        QmTimePrivate() : QObject(NULL)
        {
            if (!ifc.isValid()) {
                qWarning() << "Timed::Interface is not valid";
            }
            if (!ifc.settings_changed_connect(this, SLOT(settings_changed(const Maemo::Timed::WallClock::Info&,bool)))) {
                qWarning() << "Could not connect to Timed::Interface::settings_changed";
            }
        }

        // getRemoteTime() and get_dst() are pretty much straight from the old clockd.

        // The idea here is to change TZ and set the localtime to the remoteTime parameter.
        // Also, return time_t in the TZ timezone.
        time_t getRemoteTime(const QDateTime &dateTime, const QString &tz, struct tm *remoteTime) {
            SaveEnvTz saveEnvTz;

            time_t timet = dateTime.toTime_t();

            setenv("TZ", tz.toAscii().data(), 1);
            tzset();
            time_t ret = -1;
            if (localtime_r(&timet, remoteTime)) {
                ret = mktime(remoteTime) - (timezone - get_dst(timet));
            }

            return ret;
        }

        int get_dst(time_t tick)
        {
            struct tm  tm;
            struct tm *res;
            int        ret = 0;

            res = localtime_r(&tick, &tm);
            if (res) {
                if (tm.tm_isdst > 0) {
                    time_t t1, t2;
                    t1 = mktime(&tm);
                    tm.tm_isdst = 0;
                    t2 = mktime(&tm);
                    ret = t2 - t1;
                }
            }
            return ret;
        }

        static void timeformat_gconfkey_changed(GConfClient*, guint, GConfEntry*, gpointer data)
        {
            QmTime *time = (QmTime *)data;
            if (time)
                emit time->timeOrSettingsChanged(MeeGo::QmTimeOnlySettingsChanged);
        }

public Q_SLOTS:
        void settings_changed(const Maemo::Timed::WallClock::Info &info, bool time_changed)
        {
            Q_UNUSED(info);
            Q_UNUSED(time_changed);
            emit timeOrSettingsChanged(MeeGo::QmTimeTimeChanged);
        }

Q_SIGNALS:
        void timeOrSettingsChanged(MeeGo::QmTimeWhatChanged);
    };
}

#endif // QMTIME_P_H
