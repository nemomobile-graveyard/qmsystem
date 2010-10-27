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

#include <qm/log>
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
        connect(priv, SIGNAL(timeOrSettingsChanged(MeeGo::QmTimeWhatChanged)), this, SIGNAL(timeOrSettingsChanged(MeeGo::QmTimeWhatChanged)));

        g_type_init();
        gc = gconf_client_get_default();
        gconf_client_add_dir(gc, TIME_FORMAT_KEY, GCONF_CLIENT_PRELOAD_ONELEVEL, NULL);
        notify_id = gconf_client_notify_add(gc, TIME_FORMAT_KEY, QmTimePrivate::gconfkey_changed, this, NULL, NULL);
    }

    QmTime::~QmTime()
    {
        MEEGO_UNINITIALIZE(QmTime);
        gconf_client_notify_remove (gc, notify_id);
        gconf_client_remove_dir (gc, TIME_FORMAT_KEY, NULL);
        g_object_unref(gc);
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
        MEEGO_PRIVATE(QmTime);

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
        /*InfoReply res = priv->ifc.get_wall_clock_info_sync();
        if (res.isValid()) {
            if (res.value().flagFormat24()) {
                return QmTime::format24h;
            } else {
                return QmTime::format12h;
            }
        } else {
             qWarning() << "Could not get time format";
             return QmTime::format24h;
        }*/
    }

    /*can not set time format Unknown*/
    bool QmTime::setTimeFormat(QmTime::TimeFormat format)
    {
        MEEGO_PRIVATE(QmTime);
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
        /*WallClock::Settings s;
        if (format == QmTime::format12h) {
            //s.setFlag24(false);
            GError* error = NULL;
            gconf_client_set_string(gc, TIME_FORMAT_KEY, "12", &error);
            if (error) {
                printf("Failed to set key: %s\n", error->message);
                return false;
            }
        } else {
            //s.setFlag24(true);
            GError* error = NULL;
            gconf_client_set_string(gc, TIME_FORMAT_KEY, "24", &error);
            if (error) {
                printf("Failed to set key: %s\n", error->message);
                return false;
            }
        }
        return true;
        /*BoolReply res = priv->ifc.wall_clock_settings_sync(s);
        if (res.isValid() && res.value()) {
            return true;
        } else {
            return false;
        }*/
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
        log_debug("This method is deprecated, DO NOT USE IT!") ;
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
        log_debug("This method is deprecated, DO NOT USE IT!") ;
        MEEGO_PRIVATE(QmTime);
        InfoReply infoRes = priv->ifc.get_wall_clock_info_sync();
        if (!infoRes.isValid()) {
            return -1;
        }
#if 0
        if (infoRes.value().utcSource() == WallClock::UtcNitz &&
            infoRes.value().timezoneSource() == WallClock::TimezoneCellular) {
#else
        if (infoRes.value().utcSource() == WallClock::UtcNitz) {
#endif
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
#if 0
#include "qmtime.h"

#include "qmipcinterface.h"
#include "msystemdbus.h"
#include "symbols_p.h"

#include <QDebug>

namespace MeeGo {

DEFINEFUNC("time", int, time_get_timezone, (char *s, size_t max), (s, max), return)
DEFINEFUNC("time", int, time_set_timezone, (const char *tz), (tz), return)
DEFINEFUNC("time", int, time_get_remote, (time_t tick, const char *tz, struct tm *tm), (tick, tz, tm), return)
DEFINEFUNC("time", int, time_set_time_format, (const char *fmt), (fmt), return)
DEFINEFUNC("time", int, time_get_time_format, (char *s, size_t max), (s, max), return)
DEFINEFUNC("time", double, time_diff, (time_t t1, time_t t2), (t1, t2), return)
DEFINEFUNC("time", int, time_is_net_time_changed, (time_t *tick, char *s, size_t max), (tick, s, max), return)
DEFINEFUNC("time", int, time_get_autosync, (void), (), return)
DEFINEFUNC("time", int, time_set_autosync, (int enable), (enable), return)
DEFINEFUNC("time", int, time_is_operator_time_accessible, (void), (), return)
DEFINEFUNC("time", int, time_get_dst_usage, (time_t tick, const char *tz), (tick, tz), return)
DEFINEFUNC("time", time_t, time_get_time, (void), (), return)
DEFINEFUNC("time", int, time_get_utc_ex, (time_t tick, struct tm *tm), (tick, tm), return)
DEFINEFUNC("time", int, time_get_local_ex, (time_t tick, struct tm *tm), (tick, tm), return)
DEFINEFUNC("time", time_t, time_mktime, (struct tm *tm, const char *tz), (tm, tz), return)
DEFINEFUNC("time", int, time_get_utc_offset, (const char *tz), (tz), return)
DEFINEFUNC("time", int, time_get_synced, (void), (), return)
DEFINEFUNC("time", int, time_set_time, (time_t tick), (tick), return)
DEFINEFUNC("time", int, time_get_time_diff, (time_t tick, const char *tz1, const char *tz2), (tick, tz1, tz2), return)
DEFINEFUNC("time", int, time_get_tzname, (char *s, size_t max), (s,max), return)

class QmTimePrivate
{
    MEEGO_DECLARE_PUBLIC(QmTime)

public:
    QmTimePrivate() {
        signalIf = new QmIPCInterface(
                    SYS_CLOCK_SERVICE,
                    SYS_CLOCK_PATH,
                    SYS_CLOCK_INTERFACE);
    }

    ~QmTimePrivate() {
        delete signalIf;
    }

    QmIPCInterface *signalIf;
};

QmTime::QmTime(QObject *parent) : QObject(parent), QDateTime()
{
    MEEGO_INITIALIZE(QmTime)

    QmIPCInterface *signalIf = priv->signalIf;
    signalIf->connect(SYS_CLOCK_TIME_SIG, this, SLOT(slotTimeChanged(const qint32&)));

    // Try to load libtime
    if (!symbols_load_library("time"))
        qFatal("Unable to load libtime library");
}

QmTime::~QmTime()
{
    MEEGO_UNINITIALIZE(QmTime) ;
    symbols_unload_libraries();
}

bool QmTime::getNetTime(QDateTime& time, QString& tz)
{
    int length = 32;
    int ret = 0;
    char *buffer;
    time_t tick;
    do {
        buffer = new char[length];
        ret = q_time_is_net_time_changed(&tick, buffer, length);
        if (ret < 0) {
            delete buffer;
            return false;
        }
        length *= 2;
    } while (length < ret);

    time.setTime_t(tick);
    tz = QString(buffer);
    delete buffer;

    return true;
}

bool QmTime::setTime(const QDateTime& time)
{
    if (!(time.isValid())) {
        qWarning() << "Invalid time:" << time;
        return false;
    }
    return (q_time_set_time(time.toTime_t()) == 0) ? true : false;
}

bool QmTime::getTimezone(QString& s)
{
    int length = 32;
    int ret = 0;
    char *buffer;
    do {
        buffer = new char[length];
        ret = q_time_get_timezone(buffer, length);
        if (ret < 0) {
            delete buffer;
            return false;
        }
        length *= 2;
    } while (length < ret);

    s = QString(buffer);
    delete buffer;
    return true;
}

bool QmTime::getTZName(QString& s)
{
    int length = 6;
    int ret = 0;
    char *buffer;
    do {
        buffer = new char[length];
        ret = q_time_get_tzname(buffer, length);
        if (ret < 0) {
            delete buffer;
            return false;
        }
        length *= 2;
    } while (length < ret);

    s = QString(buffer);
    delete buffer;
    return true;
}

bool QmTime::setTimezone(const QString tz)
{
    return (q_time_set_timezone(tz.toLocal8Bit().constData()) ==0) ? true : false;
}

bool QmTime::getRemoteTime(const QDateTime& time, const QString& tz, QDateTime& remoteTime)
{
    struct tm tm;
    if (q_time_get_remote(time.toTime_t(), tz.toLocal8Bit().constData(), &tm) != 0) {
        return false;
    }

    remoteTime.setTime_t(q_time_mktime(&tm, tz.toLocal8Bit().constData()));

    return true;
}

QmTime::TimeFormat QmTime::getTimeFormat()
{
    int max = 32;
    char s[max];
    int ret = q_time_get_time_format(s, max);
    QmTime::TimeFormat tf = QmTime::format24h;

    if (ret < 0) {
        qWarning() << "Output error in time_get_time_format()";
    } else if (ret == 0) {
        qWarning() << "Too short output buffer for time_get_time_format()";
    } else {
        QString fs(s);
        if (fs.contains("%I", Qt::CaseSensitive) ||
            fs.contains("%l", Qt::CaseSensitive) ||
            fs.contains("%r", Qt::CaseSensitive)) {
            tf = QmTime::format12h;
        }
    }
    return tf;
}

bool QmTime::setTimeFormat(QmTime::TimeFormat format)
{
    int ret;
    switch (format) {
        case (int)(QmTime::format24h):
            ret = q_time_set_time_format("%R");
            break;
        case (int)(QmTime::format12h):
            ret = q_time_set_time_format("%I:%M");
            break;
        default:
            qWarning() << "Unrecognized time format";
    }
    return (ret==0) ? true : false;
}

int QmTime::getUTCOffset(const QString& tz)
{
    return q_time_get_utc_offset(tz.isEmpty() ? NULL : tz.toLocal8Bit().constData());
}

int QmTime::getDSTUsage(const QDateTime& time, const QString& tz)
{
    Return q_time_get_dst_usage(time.toTime_t(), tz.isEmpty() ? NULL : tz.toLocal8Bit().constData());
}

int QmTime::getTimeDiff(const QDateTime& time, const QString& tz1, const QString& tz2)
{
    QString tz;
    if (tz1.isEmpty() || tz2.isEmpty()) {
        getTimezone(tz);
    }
    return q_time_get_time_diff(time.toTime_t(),
                                tz1.isEmpty() ? tz.toLocal8Bit().constData() : tz1.toLocal8Bit().constData(),
                                tz2.isEmpty() ? tz.toLocal8Bit().constData() : tz2.toLocal8Bit().constData()
                                );
}

bool QmTime::setAutosync(bool enable)
{
    return (q_time_set_autosync((enable?1:0)) == 0) ? true : false;
}

int QmTime::getAutosync(void)
{
    return q_time_get_autosync();
}

int QmTime::isOperatorTimeAccessible(void)
{
    return q_time_is_operator_time_accessible();
}

void QmTime::slotTimeChanged(const qint32& time)
{
    q_time_get_synced();

    if (time == 0) {
        emit timeOrSettingsChanged(QmTimeOnlySettingsChanged);
    } else {
        emit timeOrSettingsChanged(QmTimeTimeChanged);
    }
}

} //MeeGo namespace
#endif
