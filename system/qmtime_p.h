/*!
 * @file qmtime_p.h
 * @brief Contains QmTimePrivate

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Yang Yang <ext-yang.25.yang@nokia.com>

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

#include <string>
using namespace std;

#include <QDebug>
#include <QMutex>
#include <timed/interface>

#include <gconf/gconf-client.h>

#include "qmtime.h"

#define QMTIME_SUPPORT_DEPRECATED         1
#define QMTIME_SUPPORT_UNUSED             1
#define QMTIME_SUPPORT_TIME_FORMAT        1

#if QMTIME_SUPPORT_TIME_FORMAT
#define TIME_FORMAT_KEY "/meegotouch/i18n/lc_timeformat24h"
#endif

// Signal names
static inline const char *p_signal()
{
    return SIGNAL(change_signal(MeeGo::QmTime::WhatChanged));
}

static inline const char *signal()
{
    return SIGNAL(timeOrSettingsChanged(MeeGo::QmTime::WhatChanged));
}

// Obsolete signal names
#if QMTIME_SUPPORT_DEPRECATED
static inline const char *p_signal_o()
{
    return SIGNAL(change_signal(MeeGo::QmTimeWhatChanged));
}

static inline const char *signal_o()
{
    return SIGNAL(timeOrSettingsChanged(MeeGo::QmTimeWhatChanged));
}
#endif

// Slot name
static inline const char *p_slot_timed()
{
    return SLOT(timed_signal(const Maemo::Timed::WallClock::Info &, bool));
}

namespace MeeGo {
    class QmTimePrivate;
    class QmTime;

    /** Policy regarding disconnection from time daemon */
    enum DisconnectionPolicy {
        DisconnectWhenPossible, /**< Time/settings change notification will be dropped if no QmTime object exists (default) */
        KeepConnected           /**< The connection will be kept until policy change or process exit */
    };

    /** Policy regarding disconnection from time and gconf daemons */
    enum SettingsSynchronizationPolicy {
        SynchronizeOnWrite, /**< Every call changing time settings will be followed by settings synchronization D-Bus call */
        WaitForSignal       /**< Settings will be retrieved only after arrival of the D-Bus signal */
    };
}

class MeeGo::QmTimePrivate : public QObject
{
    Q_OBJECT;
    friend class MeeGo::QmTime;

    QmTimePrivate(QObject *parent);
    virtual ~QmTimePrivate();

    // objects
    static QmTimePrivate object;
    static QmTimePrivate *get_object();
    static void unref_object();

    int counter;
    bool initialized;

    void initialize();
    void uninitialize();
    void uninitialize_v2();
    void first_run_init();

    /**
     * Get automatically guessed time zone
     *
     * @param tz  a reference to a string variable to store the result
     *
     * @note   Currently only automatic time zone guess based on cellular network information is supported, more time zone sources may be added in the future
     *
     * @return true if information succesfully retrieved and stored in the variable
     */
    bool getAutoTimezone(QString &tz);

    /**
     * Get time received from automatic time source
     *
     * @return time_t value: amount of seconds after unix epoch begin
     *         (time_t)(-1) if automatic time is not available
     */
    time_t getAutoTime();

   /**
     * Get time difference between two locations at given moment of time
     *
     * @param t          The moment at which the difference should be calculated.
     * @param location1  The time zone representing the first remote location
     * @param location2  The time zone representing the second remote location
     *
     *                   If either first or second location is empty, the current device timezone is used
     *
     * @return           Local time difference between the locations in seconds,
     *                   positive if the location1 is to the east of location2
     *                   -1 is returned on errors
     */
    int getTimeDiff(time_t t, const QString &location1, const QString &location2);

    /**
     * @brief       Set current system and RTC date and time.
     *
     * @credential  timed::TimeControl Resource token required to set the device time.
     *
     * @param  time The new system time.
     *
     * @return      True if time is successfully set.
     */
    bool setTime(time_t time);

    /**
     * Calculate local time (in current timezone)
     *
     * @param   t        time in seconds from the Unix epoch begin
     *          dt       a reference to variable to save broken down time representation
     *          p        pointer to save struct tm providing low level details, NULL if not needed
     *
     * @return  True if information successfully retrieved
     */
    static bool localTime(time_t t, QDateTime &dt, struct tm *p=NULL);

    /**
     * Get info if the device supports time updates from cellular network operator.
     *
     * @param   a reference to a boolean variable to store the result
     *
     * @return  True if information successfully retrieved
     */
    bool isOperatorTimeAccessible(bool &result);

    // Policies
    SettingsSynchronizationPolicy sync_policy;
    DisconnectionPolicy disconn_policy;

    // Cached values
    bool timed_info_valid;
    Maemo::Timed::WallClock::Info info;
#if QMTIME_SUPPORT_TIME_FORMAT
    MeeGo::QmTime::TimeFormat format;
#endif

#if QMTIME_SUPPORT_TIME_FORMAT
    // Gconf stuff
    GConfClient *gc;
    guint notify_id;
    static void gconf_change_handler(GConfClient* client, guint, GConfEntry*, gpointer data);

    static QmTime::TimeFormat parse_format_24(const char *data);
#endif

    // Timed stuff
    Maemo::Timed::Interface *timed;
    void process_timed_info(const Maemo::Timed::WallClock::Info &wc_info, bool system_time, bool need_signal);
    bool syncronize_timed_info();

    // helpers
    class TzWrapper;
    static bool remote_time(const char *tz, time_t t, QDateTime *qdt, struct tm *tm);

    void emit_signal(bool);

signals:
    void change_signal(MeeGo::QmTimeWhatChanged);

private slots:
    void timed_signal(const Maemo::Timed::WallClock::Info &wc_info, bool system_time_changed) {
        process_timed_info(wc_info, system_time_changed, true);
    }
};

class MeeGo::QmTimePrivate::TzWrapper {
    static QMutex mutex;
    string saved_tz;
    bool set_timezone;
    bool valid;
    static bool set_tz_env(const char *tz);
public:
    TzWrapper(const char *tz);
    ~TzWrapper();
    bool is_valid() {
        return valid;
    }
};

#endif // QMTIME_P_H
