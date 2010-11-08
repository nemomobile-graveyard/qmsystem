/*!
 * @file qmlocks.h
 * @brief Contains QmLocks which provides information and actions on device locks.

   <p>
   @copyright (C) 2009-2010 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
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
#ifndef QMLOCKS_H
#define QMLOCKS_H
#include "system_global.h"
#include <QtCore/qobject.h>

QT_BEGIN_HEADER

namespace MeeGo {

class QmLocksPrivate;

/**
 * @scope Nokia Meego
 *
 * @brief Provides information and actions on device locks.
 */
class MEEGO_SYSTEM_EXPORT QmLocks : public QObject
{
    Q_OBJECT
    Q_ENUMS(Lock)
    Q_ENUMS(State)

public:
    /** Things that can be locked. */
    enum Lock
    {
        Device = 0,       /**< Device lock */
        TouchAndKeyboard  /**< Touchscreen and keyboard lock */
    };

    /** Lock states */
    enum State
    {
        Unlocked = 0,     /**< Unlocked */
        Locked,            /**< Locked */
        Unknown         /**< Unknown */
    };


public:
    QmLocks(QObject *parent = 0);
    ~QmLocks();

    /**
     * Get the current lock state.
     * @param what Which lock state to request.
     * @return Current lock state for @c what.
     */
    QmLocks::State getState(QmLocks::Lock what) const;


    /**
     * Set current lock state. Note that this interface does not allow
     * to unlock device.
     * @credential devicelock::DeviceLockControl Resource token required to set the device lock state.
     * @credential mce::TKLockControl Resource token required to set the TK (touchscreen and keyboard) lock state.
     * @param what Which lock state to set
     * @param how Set @c to which state.
     * @return True on success, false otherwise.
     */
    bool setState(QmLocks::Lock what, QmLocks::State how);


    /**
     * Set device autolock timeout. Device is automatically locked
     * after the specified amount of inactivity.
     * @credential mce::DeviceLockControl Resource token required to set the device lock timeout.
     * @state Deprecated
     * @param seconds Number of seconds of inactivity after which the device is automatically locked. Value <=0 disables autolock.
     * @return True on success, false otherwise.
     */
    bool setDeviceAutolockTime(int seconds);



    /**
     * Get device autolock timeout. 
     * @state Deprecated
     * @return Timeout in seconds, value 0 means that autolock is not acticated, -1 is error
     */
    int getDeviceAutolockTime();

Q_SIGNALS:
    /**
     * Sent when some device lock state has changed.
     * @param what Which lock state was changed
     * @param how Which state the lock was changed to.
     */
    void stateChanged(MeeGo::QmLocks::Lock what, MeeGo::QmLocks::State how);


private:
    Q_DISABLE_COPY(QmLocks)
    MEEGO_DECLARE_PRIVATE(QmLocks)
};

} // namspace MeeGo

QT_END_HEADER

#endif /*QMLOCKS_H*/

// End of file