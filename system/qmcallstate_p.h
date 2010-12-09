/*!
 * @file qmcallstate_p.h
 * @brief Contains QmCallStatePrivate

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>

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

#ifndef QMCALLSTATE_P_H
#define QMCALLSTATE_P_H

#include "qmcallstate.h"
#include "qmipcinterface.h"

#if __MCE__
    #include "mce/dbus-names.h"
    #include "mce/mode-names.h"
#endif

namespace MeeGo
{

    class QmCallStatePrivate : public QObject {
        Q_OBJECT
        MEEGO_DECLARE_PUBLIC(QmCallState)

    public:
       QmCallStatePrivate(){
#if __MCE__
           signalIf = new QmIPCInterface(
                          MCE_SERVICE,
                          MCE_SIGNAL_PATH,
                          MCE_SIGNAL_IF);
           requestIf = new QmIPCInterface(
                          MCE_SERVICE,
                          MCE_REQUEST_PATH,
                          MCE_REQUEST_IF);
           signalIf->connect(MCE_CALL_STATE_SIG, this, SLOT(callStateChanged(const QString&, const QString&)));
#endif
       }

       ~QmCallStatePrivate(){
#if __MCE__
           delete requestIf;
           delete signalIf;
#endif
       }

       QmIPCInterface *requestIf;
       QmIPCInterface *signalIf;

   Q_SIGNALS:
       void stateChanged(MeeGo::QmCallState::State state, MeeGo::QmCallState::Type type);

   public Q_SLOTS:
       void callStateChanged(const QString& state, const QString& type) {
#if __MCE__
           QmCallState::State mState = QmCallState::Error;
           QmCallState::Type mType = QmCallState::Unknown;

           if (state == MCE_CALL_STATE_ACTIVE)
               mState = QmCallState::Active;
           else if (state == MCE_CALL_STATE_SERVICE)
               mState = QmCallState::Service;
           else if (state == MCE_CALL_STATE_NONE)
               mState = QmCallState::None;

           if (type == MCE_NORMAL_CALL)
               mType = QmCallState::Normal;
           else if (type == MCE_EMERGENCY_CALL)
               mType = QmCallState::Emergency;

            emit stateChanged(mState, mType);
#else
    Q_UNUSED(state);
    Q_UNUSED(type);
#endif
       }
   };

}
#endif // QMCALLSTATE_P_H
