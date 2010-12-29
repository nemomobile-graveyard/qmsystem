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

#include "qmipcinterface_p.h"

namespace MeeGo
{
    class QmLocksPrivate : public QObject
    {
        Q_OBJECT;
        MEEGO_DECLARE_PUBLIC(QmLocks)

    public:
        QmLocksPrivate() :
            mceRequestIf(0) {
            #if HAVE_MCE
                mceRequestIf = new QmIPCInterface(MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF);

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
        }

        ~QmLocksPrivate() {
            if (mceRequestIf)
                delete mceRequestIf, mceRequestIf = 0;
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

        QmLocks::State getState(QmLocks::Lock what, bool async) {
            QmLocks::State state = QmLocks::Unknown;

            if (what == QmLocks::Device) {
                qWarning() << "QmLocks::Device is not supported. Please use the DeviceLock interface.";
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
                Q_UNUSED(how);
                qWarning() << "QmLocks::Device is not supported. Please use the DeviceLock interface.";
            } else if (what == QmLocks::TouchAndKeyboard) {
                #if HAVE_MCE
                    mceRequestIf->callAsynchronously(MCE_TKLOCK_MODE_CHANGE_REQ, QmLocksPrivate::stateToString(what, how));
                    success = true;
                #else
                    Q_UNUSED(what);
                #endif /* HAVE_MCE */
            }
            return success;
        }

        QmIPCInterface *mceRequestIf;

    Q_SIGNALS:
        void stateChanged(MeeGo::QmLocks::Lock what, MeeGo::QmLocks::State how);

    private Q_SLOTS:

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

        void touchAndKeyboardStateChanged(const QString& state) {
            emit stateChanged(QmLocks::TouchAndKeyboard, stringToState(state));
        }
    };
}
#endif // QMLOCKS_P_H
