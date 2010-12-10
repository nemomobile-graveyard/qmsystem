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

#if HAVE_MCE
    #include "mce/dbus-names.h"
    #include "mce/mode-names.h"
#endif

#if HAVE_DEVICELOCK
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
#if HAVE_MCE
            signalIf = new QmIPCInterface(
                          MCE_SERVICE,
                          MCE_SIGNAL_PATH,
                          MCE_SIGNAL_IF);
            requestIf = new QmIPCInterface(
                      MCE_SERVICE,
                      MCE_REQUEST_PATH,
                      MCE_REQUEST_IF);
#endif

#if HAVE_DEVICELOCK
            DeviceLock::DeviceLockEnums::registerLockEnumerations();
#endif
            devlockIf = NULL;
#if HAVE_DEVICELOCK
            QDBusConnection::systemBus().connect(DEVLOCK_SERVICE,
                                                 DEVLOCK_PATH,
                                                 DEVLOCK_SERVICE,
                                                 DEVLOCK_SIGNAL,
                                                 this,
                                                 SLOT(deviceStateChanged(int,int)));
#endif

#if HAVE_MCE
            signalIf->connect(MCE_TKLOCK_MODE_SIG, this, SLOT(touchAndKeyboardStateChanged(const QString&)));
#endif
        }

    ~QmLocksPrivate(){
#if HAVE_MCE
        delete requestIf;
        delete signalIf;
#endif

#if HAVE_DEVICELOCK
        delete devlockIf;
#endif
    }

    static QmLocks::State stringToState(const QString &state) {
#if HAVE_MCE
        if (state == MCE_TK_LOCKED)
        {
            return QmLocks::Locked;
        } else if (state == MCE_TK_UNLOCKED)
        {
            return QmLocks::Unlocked;
        } else {
            return QmLocks::Unknown;
        }
#else
        Q_UNUSED(state);
        return QmLocks::Unknown;
#endif
    }

    static QString stateToString(QmLocks::Lock what, QmLocks::State state) {
#if HAVE_MCE
        if (what == QmLocks::TouchAndKeyboard) {
            if (state == QmLocks::Locked) {
                return MCE_TK_LOCKED;
            } else if (state == QmLocks::Unlocked) {
                return MCE_TK_UNLOCKED;
            }
        }
#else
    Q_UNUSED(what);
    Q_UNUSED(state);
#endif
        return "";
    }

#if HAVE_DEVICELOCK
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
#if HAVE_DEVICELOCK
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
#if HAVE_MCE
                list = requestIf->get(MCE_TKLOCK_MODE_GET);
                if (!list.isEmpty()) {
                    state = list[0].toString();
                    if (!state.isEmpty()) {
                        return QmLocksPrivate::stringToState(state);
                    }
                }
#else
                return QmLocks::Unknown;
#endif
                break;
           default:
                return QmLocks::Unknown;
        }

        return QmLocks::Unknown;
    }

    bool setState(QmLocks::Lock what, QmLocks::State how){

        switch (what) {
           case QmLocks::Device:
#if HAVE_DEVICELOCK
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
                Q_UNUSED(how);
                return false;
#endif
                break;
           case QmLocks::TouchAndKeyboard:
#if HAVE_MCE
                {
                    QString str = QmLocksPrivate::stateToString(what, how);
                    if (str.isEmpty()) {
                        return false;
                    }
                    return requestIf->callSynchronously(MCE_TKLOCK_MODE_CHANGE_REQ, str);
                }
#else
                Q_UNUSED(what);
                return false;
#endif
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

#if HAVE_DEVICELOCK
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
