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

#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QDBusReply>
#include <QDebug>

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
        QmLocksPrivate() :
            mceRequestIf(0),
            devlockIf(0) {
            #if HAVE_MCE
                mceRequestIf = new QDBusInterface(MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF, QDBusConnection::systemBus());

                if (!mceRequestIf->isValid()) {
                    qDebug() << "mce D-Bus interface not valid";
                }

                if (!QDBusConnection::systemBus().connect(MCE_SERVICE,
                                                          MCE_SIGNAL_PATH,
                                                          MCE_SIGNAL_IF,
                                                          MCE_TKLOCK_MODE_SIG,
                                                          this,
                                                          SLOT(touchAndKeyboardStateChanged(const QString&)))) {
                    qDebug() << "Unable to connect mce signal interface";
                }
            #endif

            #if HAVE_DEVICELOCK
                 DeviceLock::DeviceLockEnums::registerLockEnumerations();
                 devlockIf = new QDBusInterface(DEVLOCK_SERVICE, DEVLOCK_PATH, DEVLOCK_SERVICE, QDBusConnection::systemBus());

                 if (!devlockIf->isValid()) {
                     qDebug() << "devicelock D-Bus interface not valid";
                 }

                 if (!QDBusConnection::systemBus().connect(DEVLOCK_SERVICE,
                                                           DEVLOCK_PATH,
                                                           DEVLOCK_SERVICE,
                                                           DEVLOCK_SIGNAL,
                                                           this,
                                                           SLOT(deviceStateChanged(int,int)))) {
                     qDebug() << "Unable to connect devicelock signal interface";
                 }
            #endif
        }

        ~QmLocksPrivate() {
            if (mceRequestIf)
                delete mceRequestIf, mceRequestIf = 0;
            if (devlockIf)
                delete devlockIf, devlockIf = 0;
        }

        static QmLocks::State stringToState(const QString &state) {
            #if HAVE_MCE
                if (state == MCE_TK_LOCKED) {
                    return QmLocks::Locked;
                } else if (state == MCE_TK_UNLOCKED) {
                    return QmLocks::Unlocked;
                }
            #else
                Q_UNUSED(state);
            #endif
            return QmLocks::Unknown;
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

        QmLocks::State getState(QmLocks::Lock what, bool async) {
            QmLocks::State state = QmLocks::Unknown;

            if (what == QmLocks::Device) {
                #if HAVE_DEVICELOCK
                    if (async) {
                        QDBusPendingCall pcall = devlockIf->asyncCall(DEVLOCK_GET, DeviceLock::DeviceLockEnums::Device);
                        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
                        if (!QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                                         this, SLOT(didReceiveDeviceLockState(QDBusPendingCallWatcher*)))) {
                            qDebug() << "Failed to watch pending devicelock call";
                        }
                    } else {
                        QDBusReply<int> reply = devlockIf->call(DEVLOCK_GET, DeviceLock::DeviceLockEnums::Device);
                        if (reply.isValid()) {
                            state = QmLocksPrivate::stateToState((DeviceLock::DeviceLockEnums::LockState)reply.value());
                        } else {
                            qDebug() << "Failed to query devicelock";
                        }
                    }
                #endif /* HAVE_DEVICELOCK */
            } else if (what == QmLocks::TouchAndKeyboard) {
                #if HAVE_MCE
                    if (async) {
                        QDBusPendingCall pcall = mceRequestIf->asyncCall(MCE_TKLOCK_MODE_GET);
                        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pcall, this);
                        if (!QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                                         this, SLOT(didReceiveTkLockState(QDBusPendingCallWatcher*)))) {
                            qDebug() << "Failed to watch pending mce call";
                        }
                    } else {
                        QDBusReply<QString> reply = mceRequestIf->call(MCE_TKLOCK_MODE_GET);
                        if (reply.isValid()) {
                            state = QmLocksPrivate::stringToState(reply.value());
                        } else {
                            qDebug() << "Failed to query mce";
                        }
                    }
                #endif /* HAVE_MCE */
            }
            return state;
        }

        bool setState(QmLocks::Lock what, QmLocks::State how) {
            bool success = false;
            if (what == QmLocks::Device) {
                #if HAVE_DEVICELOCK
                    QDBusMessage reply = devlockIf->call(DEVLOCK_SET, DeviceLock::DeviceLockEnums::Device, stateToState(how));
                    success = (reply.type() == QDBusMessage::ReplyMessage);
                #else
                    Q_UNUSED(how);
                #endif /* HAVE_DEVICELOCK */
            } else if (what == QmLocks::TouchAndKeyboard) {
                #if HAVE_MCE
                    QDBusMessage reply = mceRequestIf->call(MCE_TKLOCK_MODE_CHANGE_REQ, QmLocksPrivate::stateToString(what, how));
                    success = (reply.type() == QDBusMessage::ReplyMessage);
                #else
                    Q_UNUSED(what);
                #endif /* HAVE_MCE */
            }
            return success;
        }

        QDBusInterface *mceRequestIf;
        QDBusInterface *devlockIf;

    Q_SIGNALS:
        void stateChanged(MeeGo::QmLocks::Lock what, MeeGo::QmLocks::State how);

    private Q_SLOTS:

        void didReceiveDeviceLockState(QDBusPendingCallWatcher *call) {
            #if HAVE_DEVICELOCK
                QDBusPendingReply<int> reply = *call;
                if (reply.isError()) {
                    return;
                }
                int state = reply.argumentAt<0>();
                emit stateChanged(QmLocks::Device, stateToState((DeviceLock::DeviceLockEnums::LockState)state));
            #endif
            call->deleteLater();
        }

        void didReceiveTkLockState(QDBusPendingCallWatcher *call) {
            #if HAVE_MCE
                QDBusPendingReply<QString> reply = *call;
                if (reply.isError()) {
                    return;
                }
                QString state = reply.argumentAt<0>();
                emit stateChanged(QmLocks::TouchAndKeyboard, stringToState(state));
            #endif
            call->deleteLater();
        }

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
