/*!
 * @file qmtime.cpp
 * @brief QmTime

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.rongas@nokia.com>
   @author Yang Yang <ext-yang.25.yang@nokia.com>

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
#include <QDBusReply>
#include <QDebug>
#include <QDateTime>

#include <gconf/gconf-client.h>
#include "qmtime_p.h"
#include "qmtime.h"


using namespace Maemo::Timed;

namespace MeeGo {

    QMutex SaveEnvTz::mtx;
    GConfClient *gc;
    guint notify_id;

    typedef QDBusReply<WallClock::Info> InfoReply;
    typedef QDBusReply<bool> BoolReply;

    QmTime::QmTime(QObject *parent) : QObject(parent)
    {
        MEEGO_INITIALIZE(QmTime);

        g_type_init();
        gc = gconf_client_get_default();
        gconf_client_add_dir(gc, TIME_FORMAT_KEY, GCONF_CLIENT_PRELOAD_ONELEVEL, NULL);
        notify_id = gconf_client_notify_add(gc,                                          /* a GConfClient */
                                            TIME_FORMAT_KEY,                             /* namespace_section: where to listen for changes */
                                            QmTimePrivate::timeformat_gconfkey_changed,  /* function to call when changes occur */
                                            this,                                        /* user data to pass to func */
                                            NULL,                                        /* function to call on user_data when the notify is removed */
                                            NULL);                                       /* the return location for an allocated GError */
    }

    QmTime::~QmTime()
    {
        gconf_client_notify_remove (gc, notify_id);
        gconf_client_remove_dir (gc, TIME_FORMAT_KEY, NULL);
        g_object_unref(gc);

        MEEGO_UNINITIALIZE(QmTime);
    }

    bool QmTime::getNetTime(QDateTime &time, QString &tz)
    {
        MEEGO_PRIVATE(QmTime);

        InfoReply res = priv->ifc.get_wall_clock_info_sync();
        if (!res.isValid()) {
            return false;
        }

        time_t value = res.value().utc(WallClock::UtcNitz);
        if (value == -1) {
            return false;
        }
        time.setTime_t(value);

        tz = res.value().timezone(WallClock::TimezoneCellular);
        if (tz.isEmpty()) {
            return false;
        }

        return true;
    }

    bool QmTime::setTime(const QDateTime &time)
    {
        MEEGO_PRIVATE(QmTime);

        WallClock::Settings s;
        s.setTimeManual(time.toTime_t());
        if (!s.check()) {
            return false;
        }
        BoolReply res = priv->ifc.wall_clock_settings_sync(s);
        if (res.isValid() && res.value()) {
            return true;
        } else {
            return false;
        }
    }

    bool QmTime::getTimezone(QString &s)
    {
        MEEGO_PRIVATE(QmTime);

        InfoReply res = priv->ifc.get_wall_clock_info_sync();
        if (!res.isValid()) {
            return false;
        }

        s = res.value().humanReadableTz();
        return true;
    }

    bool QmTime::getTZName(QString &s)
    {
        MEEGO_PRIVATE(QmTime);

        InfoReply res = priv->ifc.get_wall_clock_info_sync();
        if (!res.isValid()) {
            return false;
        }

        s = res.value().tzAbbreviation();
        return true;
    }

    bool QmTime::setTimezone(const QString tz)
    {
        MEEGO_PRIVATE(QmTime);

        WallClock::Settings s;
        s.setTimezoneManual(tz);
        if (!s.check()) {
            return false;
        }
        BoolReply res = priv->ifc.wall_clock_settings_sync(s);
        if (res.isValid() && res.value()) {
            return true;
        } else {
            return false;
        }

    }

    bool QmTime::getRemoteTime(const QDateTime &time, const QString &tz, QDateTime &remoteTime)
    {
        MEEGO_PRIVATE(QmTime);

        struct tm remoteTm;
        time_t res = priv->getRemoteTime(time, tz, &remoteTm);
        if (res != -1) {
            remoteTime.setTime_t(mktime(&remoteTm));
            return true;
        } else {
            return false;
        }
    }

    QmTime::TimeFormat QmTime::getTimeFormat()
    {
        gchar *ret = gconf_client_get_string(gc, TIME_FORMAT_KEY, NULL);
        if (ret == NULL)
            return QmTime::formatUnknown;
        if (strncmp(ret, "12", sizeof(ret)) == 0) {
            return QmTime::format12h;
        } else if (strncmp(ret, "24", sizeof(ret)) == 0) {
            return QmTime::format24h;
        }
        g_free(ret);
        return QmTime::formatUnknown;
    }

    /*can not set time format Unknown*/
    bool QmTime::setTimeFormat(QmTime::TimeFormat format)
    {
        WallClock::Settings s;
        if (format == QmTime::format12h) {
            s.setFlag24(false);
            if (gconf_client_set_string(gc, TIME_FORMAT_KEY, "12", NULL)) {
                emit timeOrSettingsChanged(QmTimeOnlySettingsChanged);
                return true;
            }
        } else if (format == QmTime::format24h) {
            s.setFlag24(true);
            if (gconf_client_set_string(gc, TIME_FORMAT_KEY, "24", NULL)) {
                emit timeOrSettingsChanged(QmTimeOnlySettingsChanged);
                return true;
            }
        }
        return false;
    }

    int QmTime::getUTCOffset(const QString &tz)
    {
        MEEGO_PRIVATE(QmTime);

        struct tm timetm;
        if (priv->getRemoteTime(QDateTime::currentDateTime(), tz, &timetm) != -1) {
            return timetm.tm_gmtoff;
        } else {
            return 0;
        }
    }

    int QmTime::getDSTUsage(const QDateTime &time, const QString &tz)
    {
        MEEGO_PRIVATE(QmTime);

        struct tm timetm;
        if (priv->getRemoteTime(time, tz, &timetm) != -1) {
            return timetm.tm_isdst;
        } else {
            return 0;
        }
    }

    int QmTime::getTimeDiff(const QDateTime &time, const QString &tz1, const QString &tz2)
    {
        MEEGO_PRIVATE(QmTime);

        struct tm tm1;
        struct tm tm2;
        time_t remote1 = priv->getRemoteTime(time, tz1, &tm1);
        time_t remote2 = priv->getRemoteTime(time, tz2, &tm2);

        return remote1 - remote2;
    }

    enum QmTime::AutoSystemTimeStatus QmTime::autoSystemTime()
    {
      QDBusReply<Maemo::Timed::WallClock::Info> res = priv_func()->ifc.get_wall_clock_info_sync() ;
      if (not res.isValid())
        return AutoSystemTimeUnknown ;
      return res.value().flagTimeNitz() ? AutoSystemTimeOn : AutoSystemTimeOff ;
    }

    bool QmTime::setAutoSystemTime(enum QmTime::AutoSystemTimeStatus new_status)
    {
      Maemo::Timed::WallClock::Settings s ;
      switch(new_status)
      {
        case AutoSystemTimeOn:
          s.setTimeNitz() ;
          break ;
        case AutoSystemTimeOff:
          s.setTimeManual() ;
          break ;
        default:
          return false ;
      }
      QDBusReply<bool> res = priv_func()->ifc.wall_clock_settings_sync(s) ;
      return res.isValid() and res.value() ;
    }

    enum QmTime::AutoTimeZoneStatus QmTime::autoTimeZone()
    {
      QDBusReply<Maemo::Timed::WallClock::Info> res = priv_func()->ifc.get_wall_clock_info_sync() ;
      if (not res.isValid())
        return AutoTimeZoneUnknown ;
      return res.value().flagLocalCellular() ? AutoTimeZoneOn : AutoTimeZoneOff ;
    }

    bool QmTime::setAutoTimeZone(enum QmTime::AutoTimeZoneStatus new_status)
    {
      Maemo::Timed::WallClock::Settings s ;
      switch(new_status)
      {
        case AutoTimeZoneOn:
          s.setTimezoneCellular() ;
          break ;
        case AutoTimeZoneOff:
          s.setTimezoneManual("") ;
          break ;
        default:
          return false ;
      }
      QDBusReply<bool> res = priv_func()->ifc.wall_clock_settings_sync(s) ;
      return res.isValid() and res.value() ;
    }

    /*!
     * Autosync: the guys are expecting that both time and timezone are from
     *           cellular network.
     */
    bool QmTime::setAutosync(bool enable)
    {
        qDebug() << "This method is deprecated, DO NOT USE IT!";

        MEEGO_PRIVATE(QmTime);
        WallClock::Settings s;
        if (enable) {
            s.setTimeNitz();
            s.setTimezoneCellular();
        } else {
            s.setTimeManual();
            s.setOffsetManual();
        }
        BoolReply res = priv->ifc.wall_clock_settings_sync(s);
        if (!res.isValid() || !res.value()) {
            return false;
        } else {
            return true;
        }
    }

    /*!
     * Should we return 1 if we have time_nitz and local_cellular flags set,
     * or only if our current sources are UtcNitz and TimezoneCellular?
     */
    int QmTime::getAutosync()
    {
        qDebug() << "This method is deprecated, DO NOT USE IT!";

        MEEGO_PRIVATE(QmTime);
        InfoReply infoRes = priv->ifc.get_wall_clock_info_sync();
        if (!infoRes.isValid()) {
            return -1;
        }
        if (infoRes.value().utcSource() == WallClock::UtcNitz) {
            return 1;
        } else {
            return 0;
        }
    }

    /*!
     * Should this somehow return always 1 on Dali?
     */
    int QmTime::isOperatorTimeAccessible()
    {
        MEEGO_PRIVATE(QmTime);
        InfoReply infoRes = priv->ifc.get_wall_clock_info_sync();
        if (!infoRes.isValid()) {
            return -1;
        }
        if (infoRes.value().utcAvailable(WallClock::UtcNitz)) {
            return 1;
        } else {
            return 0;
        }
    }
}
