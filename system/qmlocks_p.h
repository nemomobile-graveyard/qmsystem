/*!
 * @file qmlocks_p.h
 * @brief Contains QmLocksPrivate

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
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

#ifndef QMLOCKS_P_H
#define QMLOCKS_P_H

#include "qmlocks.h"
#include "qmipcinterface.h"
#include "mce/dbus-names.h"
#include "mce/mode-names.h"

#if __DEVICELOCK__
#include "devicelock/devicelock.h"
#endif

namespace MeeGo
{
    class QmLocksPrivate : public QObject
    {
        Q_OBJECT;
        MEEGO_DECLARE_PUBLIC(QmLocks)

    public:
        QmLocksPrivate(){
            signalIf = new QmIPCInterface(
                          MCE_SERVICE,
                          MCE_SIGNAL_PATH,
                          MCE_SIGNAL_IF);
            requestIf = new QmIPCInterface(
                      MCE_SERVICE,
                      MCE_REQUEST_PATH,
                      MCE_REQUEST_IF);

#if __DEVICELOCK__
            DeviceLock::DeviceLockEnums::registerLockEnumerations();
#endif
            devlockIf = NULL;
#if __DEVICELOCK__
            QDBusConnection::systemBus().connect(DEVLOCK_SERVICE,
                                                 DEVLOCK_PATH,
                                                 DEVLOCK_SERVICE,
                                                 DEVLOCK_SIGNAL,
                                                 this,
                                                 SLOT(deviceStateChanged(int,int)));
#endif
            signalIf->connect(MCE_TKLOCK_MODE_SIG, this, SLOT(touchAndKeyboardStateChanged(const QString&)));
        }

    ~QmLocksPrivate(){
        delete requestIf;
        delete signalIf;
        delete devlockIf;
    }

    static QmLocks::State stringToState(const QString &state) {
        if (state == MCE_TK_LOCKED)
        {
            return QmLocks::Locked;
        } else if (state == MCE_TK_UNLOCKED)
        {
            return QmLocks::Unlocked;
        } else {
            return QmLocks::Unknown;
        }
    }

    static QString stateToString(QmLocks::Lock what, QmLocks::State state) {
        if (what == QmLocks::TouchAndKeyboard) {
            if (state == QmLocks::Locked) {
                return MCE_TK_LOCKED;
            } else if (state == QmLocks::Unlocked) {
                return MCE_TK_UNLOCKED;
            }
        }
        return "";
    }

#if __DEVICELOCK__
    static QmLocks::State stateToState(DeviceLock::DeviceLockEnums::LockState state) {
        switch (state) {
        case DeviceLock::DeviceLockEnums::Unlocked:
            return QmLocks::Unlocked;
        case DeviceLock::DeviceLockEnums::Locked:
            return QmLocks::Locked;
        default:
            return QmLocks::Unknown;
        }
    }

    static DeviceLock::DeviceLockEnums::LockState stateToState(QmLocks::State state) {
        switch (state) {
        case QmLocks::Unlocked:
            return DeviceLock::DeviceLockEnums::Unlocked;
        case QmLocks::Locked:
            return DeviceLock::DeviceLockEnums::Locked;
        default:
            return DeviceLock::DeviceLockEnums::Undefined;
        }
    }
#endif

    QmLocks::State getState(QmLocks::Lock what) {

        QString state;
        QList<QVariant> list;
        switch (what){
           case QmLocks::Device:
#if __DEVICELOCK__
            {
                QDBusMessage call = QDBusMessage::createMethodCall(DEVLOCK_SERVICE, DEVLOCK_PATH, DEVLOCK_SERVICE, DEVLOCK_GET);
                QList<QVariant> args;
                args.prepend(DeviceLock::DeviceLockEnums::Device);
                call.setArguments(args);
                QDBusMessage ret = QDBusConnection::systemBus().call(call, QDBus::Block, 10*1000);
                if (ret.type() == QDBusMessage::ReplyMessage) {
                    list = ret.arguments();
                }
                if (!list.isEmpty()) {
                    bool toIntOk = false;
                    int val = list[0].toInt(&toIntOk);
                    if (toIntOk) {
                        return QmLocksPrivate::stateToState((DeviceLock::DeviceLockEnums::LockState)val);
                    }
                }
            }
#else
                return QmLocks::Unknown;
#endif
                break;
           case QmLocks::TouchAndKeyboard:
                list = requestIf->get(MCE_TKLOCK_MODE_GET);
                if (!list.isEmpty()) {
                    state = list[0].toString();
                    if (!state.isEmpty()) {
                        return QmLocksPrivate::stringToState(state);
                    }
                }
                break;
           default:
                return QmLocks::Unknown;
        }

        return QmLocks::Unknown;
    }

    bool setState(QmLocks::Lock what, QmLocks::State how){

        switch (what) {
           case QmLocks::Device:
#if __DEVICELOCK__
                {
                 QList<QVariant> res;
                    QDBusMessage call = QDBusMessage::createMethodCall(DEVLOCK_SERVICE, DEVLOCK_PATH, DEVLOCK_SERVICE, DEVLOCK_SET);
                    QList<QVariant> args;
                    args.prepend(DeviceLock::DeviceLockEnums::Device);
                    args.append(stateToState(how));
                    call.setArguments(args);
                    QDBusMessage ret = QDBusConnection::systemBus().call(call, QDBus::Block, 10*1000);
                    if (ret.type() == QDBusMessage::ReplyMessage) {
                        res = ret.arguments();
                    }
                    if (res.isEmpty() || !res.first().toBool()) {
                        return false;
                    } else {
                        return true;
                    }
                }
#else
                return false;
#endif
                break;
           case QmLocks::TouchAndKeyboard:
                {
                    QString str = QmLocksPrivate::stateToString(what, how);
                    if (str.isEmpty()) {
                        return false;
                    }
                    return requestIf->callSynchronously(MCE_TKLOCK_MODE_CHANGE_REQ, str);
                }
                break;
           default:
                return false;
        }
    }

    QmIPCInterface *requestIf;
    QmIPCInterface *signalIf;
    QmIPCInterface *devlockIf;

Q_SIGNALS:
    void stateChanged(MeeGo::QmLocks::Lock what, MeeGo::QmLocks::State how);

private Q_SLOTS:

#if __DEVICELOCK__
    void deviceStateChanged(int device, int state) {
        if (device == DeviceLock::DeviceLockEnums::Device) {
            emit stateChanged(QmLocks::Device, stateToState((DeviceLock::DeviceLockEnums::LockState)state));
        }
    }
#endif

    void touchAndKeyboardStateChanged(const QString& state) {
        emit stateChanged(QmLocks::TouchAndKeyboard, stringToState(state));
    }

};
}
#endif // QMLOCKS_P_H
