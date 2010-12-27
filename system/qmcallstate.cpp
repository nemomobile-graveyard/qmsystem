/*!
 * @file qmcallstate.cpp
 * @brief QmCallState

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>

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
#include "qmcallstate.h"
#include "qmcallstate_p.h"

namespace MeeGo {

QmCallState::QmCallState(QObject *parent) : QObject(parent) {
    MEEGO_INITIALIZE(QmCallState);
    connect(priv, SIGNAL(stateChanged(MeeGo::QmCallState::State, MeeGo::QmCallState::Type)),
            this, SIGNAL(stateChanged(MeeGo::QmCallState::State,MeeGo::QmCallState::Type)));
}

QmCallState::~QmCallState(){
    MEEGO_UNINITIALIZE(QmCallState);
}

QmCallState::State QmCallState::getState() const{
    State mState = Error;

#if HAVE_MCE
    MEEGO_PRIVATE_CONST(QmCallState)
    QString state;

    QList<QVariant> resp;
    resp = priv->requestIf->get(MCE_CALL_STATE_GET);

    if (resp.count() != 2) {
        return Error;
    }

    state = resp[0].toString();

    if (state == MCE_CALL_STATE_ACTIVE)
        mState = Active;
    else if (state == MCE_CALL_STATE_SERVICE)
        mState = Service;
    else if (state == MCE_CALL_STATE_NONE)
        mState = None;
#endif

    return mState;
}

QmCallState::Type QmCallState::getType() const {
    Type mType = Unknown;

#if HAVE_MCE
    MEEGO_PRIVATE_CONST(QmCallState)
    QString type;

    QList<QVariant> resp;
    resp = priv->requestIf->get(MCE_CALL_STATE_GET);

    if (resp.count() != 2)
        return Unknown;

    type = resp[1].toString();

    if (type == MCE_NORMAL_CALL)
        mType = Normal;
    else if (type == MCE_EMERGENCY_CALL)
        mType = Emergency;
#endif

    return mType;
}

bool QmCallState::setState(QmCallState::State state, QmCallState::Type type) {
#if HAVE_MCE
    MEEGO_PRIVATE(QmCallState)

    QString mState;
    QString mType;

    if (state == Active)
        mState = MCE_CALL_STATE_ACTIVE;
    else if (state == Service)
        mState = MCE_CALL_STATE_SERVICE;
    else if (state == None)
        mState = MCE_CALL_STATE_NONE;
    else
        return false;

    if (type == Normal)
        mType = MCE_NORMAL_CALL;
    else if (type == Emergency)
        mType = MCE_EMERGENCY_CALL;
    else
        return false;

    priv->requestIf->callAsynchronously(MCE_CALL_STATE_CHANGE_REQ, mState, mType);
    return true;
#else
    Q_UNUSED(state);
    Q_UNUSED(type);
    return false;
#endif

}



} // namespace MeeGo
