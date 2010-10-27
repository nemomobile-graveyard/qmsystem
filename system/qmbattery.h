/*!
 * @file qmbattery.h
 * @brief Contains QmBattery.

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Matti Halme <matthalm@esdhcp04350.research.nokia.com>
   @author Na Fan <ext-na.2.fan@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
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
#ifndef QMBATTERY_H
#define QMBATTERY_H


#include <QtCore/qobject.h>
#include "system_global.h"
#include <QList>
#include <QScopedPointer>

QT_BEGIN_HEADER

namespace MeeGo {

class QmBatteryPrivate;

/*!
 *
 * @scope Nokia Meego
 *
 * @class QmBattery
 * @brief QmBattery provides information on device battery status.
 */
class MEEGO_SYSTEM_EXPORT QmBattery : public QObject
{
    Q_OBJECT
    Q_ENUMS(Level)                        /* Depreceated, use BatteryState */
    Q_ENUMS(BatteryState)
    Q_ENUMS(RemainingTimeMode)
    Q_ENUMS(State)                        /* Depreceated, use ChargingState */
    Q_ENUMS(ChargingState)
    Q_ENUMS(Period)
    Q_ENUMS(ChargerType)
    Q_PROPERTY(State state READ getState) /* Depreceated, use ChargingState */
    Q_PROPERTY(ChargingState state READ getChargingState)
    Q_PROPERTY(Level level READ getLevel) /* Depreceated, use BatteryState */
    Q_PROPERTY(ChargerType type READ getChargerType)

public:

    //! Battery charge level states, depreceated (use BatteryState)
    enum Level
    {
        LevelFull = 0,
        LevelLow,
        LevelCritical
    };

    enum BatteryState
    {
        StateEmpty = 0,  //!< Battery empty (system being shut down)
        StateLow,        //!< Battery low (warnings issued to the user)
        StateOK,         //!< Battery OK
        StateFull,       //!< Battery full
        StateError       //!< Battery state not known
    };

    //! Battery charging states
    enum ChargingState
    {
        StateNotCharging = 0, //!< Not charging
        StateCharging,        //!< Charging
        StateChargingFailed   //!< Charging error, e.g. unsupported charger
    };

    typedef ChargingState State;                           /* Depreceated */

    //! The mode in which the remaining time is to be calculated
    enum RemainingTimeMode
    {
        NormalMode,       //!< In normal mode
        PowersaveMode     //!< In powersave mode
    };

    //! The type of charger connected
    enum ChargerType
    {
        Unknown = -1,        //!< Unknown charger
        None,                //!< No  charger connected
        Wall,                //!< Wall charger
        USB_500mA,           //!< USB with 500mA output
        USB_100mA            //!< USB with 100mA output
    };

    //! Battery current measurement periods
    enum Period
    {
        RATE_250ms = 0,         //!< Measure every 250ms
        RATE_1000ms,            //!< Measure every 1000ms
        RATE_5000ms             //!< Measure every 5000ms
    };

public:

    QmBattery(QObject *parent = 0);
    virtual ~QmBattery();

    /*!
     * @brief Get the battery nominal (maximum) capasity.
     *
     * @returns  The battery nominal (maximum) capasity (mAh)
     */
    int getNominalCapacity() const;
    
    /*!
     * @brief Get the battery state.
     *
     * @returns  The battery state as QmBattery::BatteryState
     */
    BatteryState getBatteryState() const;

    /*!
     * @brief Get remaining battery capacity as mAh
     *
     * @returns  Remaining battery capacity as mAh
     */
    int getRemainingCapacitymAh() const;
    
    /*!
     * @brief Get remaining battery capacity as a percentage.
     *
     * @returns  Battery level in percents [0 - 100]
     */
    int getRemainingCapacityPct() const;
    
    /*!
     * @brief Get remaining battery capacity as number of bars or 0 if battery state
     * is low.
     *
     * @returns  Battery level as number of bars [0 - getMaxbars()]
     */
    int getRemainingCapacityBars() const;
    
    /*!
     * @brief Get the maximum number of battery bars.
     *
     * @returns  The maximum value returned by getRemainingCapacityBars()
     */
    int getMaxBars() const;
    
    /*!
     * @brief Get battery voltage
     *
     * @returns  The battery voltage (mV)
     */
    int getVoltage() const;

    /*!
     * @brief Get the amount of current flowing out from the battery
     * @details Get the amount of current flowing out from the battery (a short term
     * averge). Positive current means discharging and negative current means
     * charging.
     *
     * @returns  The battery current (mA)
     */
    int getBatteryCurrent() const;

    /*!
     * @brief Get the cumulative amount of battery current flowing out from the
     * battery (the coulomb counter).
     *
     * @details The reference point of the cumulative battery current is undefined and
     * only differences of the returned values are meningful. If the returned
     * value is bigger than the previously returned value, the battery has
     * discharged. If the returned value is smaller than previously returned
     * value, the battery has been charged.
     *
     * @returns cumulative battery current (mAs)
     */
    int getCumulativeBatteryCurrent() const;

    /*!
     * @brief Get currently connected charger type (or None).
     *
     * @returns  Charger type as QmBattery::ChargerType
     */
    ChargerType getChargerType() const;

    /*!
     * @brief Get the charging state (e.g. on / off / failed)
     *
     * @returns  The charging state as QmBattery::ChargingState
     */
    ChargingState getChargingState() const;

    /*!
     * @brief Get the remaining charging time.
     *
     * @returns The charging time in seconds if charging or -1 if not charging
     */
    int  getRemainingChargingTime() const;

    /*!
     * @brief Start battery current measurement.
     *
     * @param rate  The rate of sending the signal (batteryCurrent)
     *              Use enums (RATE_250ms, RATE_1000ms, RATE_5000ms)
     *
     * @retval  TRUE   success
     * @retval  FALSE  failure
     */
    bool startCurrentMeasurement(Period rate);

    /*!
     * @brief Stop battery current measurement.
     *
     * @retval  TRUE   success
     * @retval  FALSE  failure
     */
    bool stopCurrentMeasurement();

    /*!
     * @brief Get the remaining talk time or -1 if not known.
     *
     * @param mode: (PowersaveMode/Normal ) mode in which the remaining
     *               time is to be estimated.
     *
     * @returns Talk time in seconds
     */
    int getRemainingTalkTime(RemainingTimeMode mode) const;

    /*!
     * @brief Get the remaining idle time or -1 if not known.
     *
     * @param mode: (PowersaveMode/Normal ) mode in which the remaining
     *               time is to be estimated.
     *
     * @returns Idle time in seconds
     */
    int getRemainingIdleTime(RemainingTimeMode mode) const;


    /*!
     * @brief Depreceated, use getRemainingCapacityPct
     * @state Deprecated
     */
    int getBatteryEnergyLevel() const;

    /*!
     * @brief Depreceated, use getBatteryState
     * @state Deprecated
     */
    Level getLevel() const;

    /*!
     * @brief Depreceated, use getChargingState
     * @state Deprecated
     */
    State getState() const;

Q_SIGNALS:

    /*!
     * @brief Sent when battery state has changed.
     *
     * @param batteryState  new battery state
     */
    void batteryStateChanged(MeeGo::QmBattery::BatteryState batteryState);

    /*!
     * @brief Sent when the remaining capacity percentage or bar count changes
     *
     * @param  percentage the new remaining capacity a percentage
     * @param  bars       the new remaining capacity as number of bars
     */
    void batteryRemainingCapacityChanged(int percentage, int bars);

    /*!
     * @brief Sent when battery charging state has changed.
     *
     * @param chargingState  the new charging state
     */
    void chargingStateChanged(MeeGo::QmBattery::ChargingState chargingState);
    
    /*!
     * @brief Sent when a charger event has occurred (charger plugged / unplugged).
     *
     * @param chargerType  the new connected charger type (or None)
     */
    void chargerEvent(MeeGo::QmBattery::ChargerType chargerType);

    /*!
     * @brief Sent at desired interval when battery current measurement is enabled
     * (see startCurrentMeasurement)
     *
     * @param current in mA
     */
    void batteryCurrent(int current);

    /*!
     * @brief Depreceated, use batteryRemainingCapacityChanged
     * @state Deprecated
     */
    void batteryEnergyLevelChanged(int percentage);

    /*!
     * @brief Depreceated, use chargingStateChanged
     * @state Deprecated
     */
    void batteryStatusChanged(MeeGo::QmBattery::State);

    /*!
     * @brief Depreceated, use batteryStateChanged
     * @state Deprecated
     */
    void batteryLevelChanged(MeeGo::QmBattery::Level level);

private:
    Q_DISABLE_COPY(QmBattery)
    friend class QmBatteryPrivate;
    QScopedPointer<QmBatteryPrivate> pimpl_;
};

} // MeeGo namespace

QT_END_HEADER

#endif /*QMBATTERY_H*/

// End of file
