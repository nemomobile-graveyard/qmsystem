/*!
 * @file qmtime.h
 * @brief Contains QmTime which provides interface for interacting with system clock.

   <p>
   @copyright (C) 2009-2010 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.rongas.nokia.com>

   @scope Nokia Meego

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

#ifndef QMTIME_H
#define QMTIME_H

#include "system_global.h"
#include <QtCore/qobject.h>
#include <QTime>
#include <QDate>
#include <QDateTime>
#include <time.h>

QT_BEGIN_HEADER

namespace MeeGo {

class QmTimePrivate;

/** Enumeration for change events for QmTime. */
enum QmTimeWhatChanged
{
    QmTimeTimeChanged = 0,      /**< Time (and settings) changed */
    QmTimeOnlySettingsChanged   /**< Settings changed */
};
Q_ENUMS(QmTimeWhatChanged);

/**
 * @scope Nokia Meego
 *
 * @brief Provides interface for interacting with system clock.
 *
 * All functions of QDateTime are accessible and should be used whenever
 * applicable. Setting the time should always be done through QmTime to ensure
 * proper signalling to other applications.
 */
class MEEGO_SYSTEM_EXPORT QmTime : public QObject, public QDateTime
{
    Q_OBJECT;
    Q_ENUMS(TimeFormat);

  public:

    QmTime(QObject *parent = 0);
    ~QmTime();

    /** Format of time */
    enum TimeFormat
    {
        format24h = 0, /**< 24h format  (\c 14:53 ) */
        format12h,      /**< 12h format  (\c  2:53 ) */
        formatUnknown
    };

    /**
     * Get current time format
     *
     * @return      <tt>QmTime::format24h</tt> or  <tt>QmTime::format12h</tt> depending on current device settings
     */
    QmTime::TimeFormat getTimeFormat();

    /**
     * Set current time format (12h/24h) setting of the device
     *
     * @credential timed::TimeControl Resource token required to set the device time format.
     *
     * @param      New format setting
     *
     * @return     True if successfully set
     */
    bool setTimeFormat(QmTime::TimeFormat format);

    /**
     * Get the current time supplied by cellular network operator (which is not nesessarily the current system time) relatively to GMT timezone
     * and the current timezone guessed on the basis of information supplied by the cellular network operator.
     *
     * @param time Reference to the structure to store the current network time. If the network time is not known,
     *             an invalid structure is stored. If the network time is known, the property timeSpec() is set to Qt::UTC
     *
     * @param tz   Reference to QString where to store network timezone. A null string is stored, if the timezone could not be guessed.
     *
     * @return     True is always returned.
     */
    bool getNetTime(QDateTime& time, QString& tz);

    /**
     * @brief       Set current system and RTC date and time.
     *
     * @credential  timed::TimeControl Resource token required to set the device time.
     *
     * @param  time The new system time. The timeSpec() property of this structure has to be set to Qt::UTC
     *
     * @return      True if time is successfully set.
     */
    bool setTime(const QDateTime& time);

    /**
     * Set current time zone.
     * @credential timed::TimeControl Resource token required to set the device time zone.
     *
     * @param tz Time zone variable.
     *           The following two formats are accepted:
     *
     *           1) Geographical entry in the Olson database.
     *              All these entries are listed in the third column of
     *              <tt>/usr/share/zoneinfo/zone.tab</tt> file.
     *              Setting the zone in this format will enable automatic DST switch.
     *
     *           2) Zones with a fixed offset relatively to UTC in 15 minutes steps.
     *              The string is matching the following regular expression:
     *              ^Fixed/UTC([-+](0|[1-9]\d*)(:(15|30|45))?)?$
     *              The value of the offset can't exceed the interval [-14,+12]. // XXX check it from wikipedia
     *              The positive offsets are to the East of Greenwich.
     *              Setting the zone in this format will disable automatic DST switch.
     *
     *           The zone description in one of the above formats can be prepended by a colon ':' sign.
     *           The colon will be silently ignored.
     *
     * @return True if zone was succesfully set, or false if not.
     */
    bool setTimezone(const QString tz);

    /**
     * Get current time zone.
     *
     * @param   a reference to a string variable to store the result
     *
     *          Depending on the current auto_dst setting will return either the geographical Olson database entry
     *          or the string describing a fixed GMT offset zone as explained in the documentation of setTimezone()
     *          method above
     *
     * @return  True is always returned.
     */
    bool getTimezone(QString& s);

    /**
     * Get current time zone abbrevation
     * (see http://www.timeanddate.com/library/abbreviations/timezones/ for some examples)
     *
     * @param   a reference to a string variable to store the result
     *
     * @return  True is always returned.
     */
    bool getTZName(QString& s);

    /**
     * Calculate time at given remote location at given moment
     *
     * @param moment     The moment at which the time should be calculated.
     * @param location   The time zone representing the remote location
     *
     *                   Every string satisfying the conditions for the
     *                   parameter of the setTimezone() method listed above is
     *                   accepted as the 'location' parameter. Beside of that
     *                   the remote timezone can be given as a string satisfying the POSIX
     *                   rules for TZ environment variable, see for details:
     *                   http://www.opengroup.org/onlinepubs/009695399/basedefs/xbd_chap08.html
     *
     *                   The moment is given as a QDateTime structure. Depending on its timeSpec()
     *                   property it's being interpreted in diffenert ways:
     *
     *                   *) if moment.timeSpec() is Qt::LocalTime, the moment structure is representing
     *                      the local time in the current device timezone.
     *
     *                   *) if moment.timeSpec() is Qt::UTC, the moment structure is representing broken down
     *                      time according to GMT timezone.
     *
     *                   *) for every other value of moment.timeSpec() the behaviour of this function
     *                      is unpredictable (most possibly it will return false)
     *
     *
     * @param remoteTime Supplied QDateTime for storing the result
     *
     *                   The remoteTime structure will be filled with the broken down representation of
     *                   the given time relatively of given timezone. The timeSpec() property will be set to Qt::LocalTime.
     *
     * @return           true if the result is sucessfuly calculated, return false and leave remoteTime structure unchanged else.
     */
    bool getRemoteTime(const QDateTime &moment, const QString &location, QDateTime &remoteTime);


    /**
     * This function is obsolete and should not be used,
     * the behaviour is unredictable.
     *
     * @note think about using of the method getTimeDiff()
     *       with the last parameter "Fixed/UTC"
     *
     */
    int getUTCOffset(const QString &);

    /**
     * Query if daylight saving time is in use at given time and location
     *
     * @param moment     The moment at which the query should be performed
     * @param location   The time zone representing the remote location
     *
     *                   See the getRemoteTime() method for explanation of parameters
     *                   If location is empty, the current device timezone is used
     *
     * @return     Nonzero if daylight savings time is in effect, zero if not, -1 if error
     */
    int getDSTUsage(const QDateTime &moment, const QString &location) ;

    /**
     * Get time difference between two locations at given moment of time
     *
     * @param moment     The moment at which the difference should be calculated.
     * @param location1  The time zone representing the first remote location
     * @param location2  The time zone representing the second remote location
     *
     *                   See the getRemoteTime() method for explanation of parameters
     *                   If either first or second location is empty, the current device timezone is used
     *
     * @return           Local time difference between the locations in seconds,
     *                   positive if the location1 is to the east of location2
     */
    int getTimeDiff(const QDateTime &moment, const QString &location1, const QString &location2) ;

    /**
     * Set the state of automatic system time setting based on the network time information supplied by operator.
     *
     * @credential timed::TimeControl Resource token required to set the network time autosync mode.
     *
     * @note    This method doesn't affect automatic setting of time zones
     *
     * @param   enable true to enable, false to disable
     *
     * @return      True if successfully set
     */
    enum AutoSystemTimeStatus { AutoSystemTimeOn = 1, AutoSystemTimeOff = 0, AutoSystemTimeUnknown = -1 } ;

    /**
     * Get the state of automatic system time setting
     *
     * @note   Currently only automatic time setting based on network operator NITZ signal is supported, more time sources may be added in the future
     *
     * @return  AutoSystemTimeOn if enabled, AutoSystemTimeOff if disabled, AutoSystemTimeUnknown on errors
     */
    enum AutoSystemTimeStatus autoSystemTime() ;

    /**
     * Set the state of automatic system time setting
     *
     * @credential timed::TimeControl Resource token required to set the network time autosync mode.
     *
     * @note   This method doesn't affect automatic setting of time zones
     *
     * @note   Currently only automatic time setting based on network operator NITZ signal is supported, more time sources may be added in the future
     *
     * @param  new_status AutoSystemTimeOn to enable, AutoSystemTimeOff to disable
     *
     * @return True if successfully set
     */
    bool setAutoSystemTime(enum AutoSystemTimeStatus new_status) ;

    enum AutoTimeZoneStatus { AutoTimeZoneOn = 1, AutoTimeZoneOff = 0, AutoTimeZoneUnknown = -1 } ;

    /**
     * Get the state of automatic time zone guessing
     *
     * @note   Currently only automatic time zone setting based on cellular network information is supported, more time zone sources may be added in the future
     *
     * @return  AutoTimeZoneOn if enabled, AutoTimeZoneOff if disabled, AutoTimeZoneUnknown on errors
     */
    enum AutoTimeZoneStatus autoTimeZone() ;

    /**
     * Set the state of automatic time zone guessing
     *
     * @credential timed::TimeControl Resource token required to set the network time autosync mode.
     *
     * @note   This method doesn't affect automatic setting of system time
     *
     * @note   Currently only automatic time zone setting based on cellular network information is supported, more time zone sources may be added in the future
     *
     * @param  new_status AutoTimeZoneOn to enable, AutoTimeZoneOff to disable
     *
     * @return True if successfully set
     */
    bool setAutoTimeZone(enum AutoTimeZoneStatus new_status) ;


    /**
     * This method is deprecated, DO NOT USE IT!
     */
    bool setAutosync(bool enable);

    /**
     * This method is deprecated, DO NOT USE IT!
     */
    int getAutosync();

    /**
     * Get info if the device supports network time updates.
     *
     * @return  Nonzero if accessible, 0 if not, -1 if error
     */
    int isOperatorTimeAccessible(void);

Q_SIGNALS:
    /**
     * Sent when system time or settings have changed.
     * @param what What has changed
     */
    void timeOrSettingsChanged(MeeGo::QmTimeWhatChanged what);

private:
    Q_DISABLE_COPY(QmTime)
    MEEGO_DECLARE_PRIVATE(QmTime)   
};

} // MeeGo namespace

QT_END_HEADER

#endif /* QMTIME_H */

// End of file
//
