/*!
 * @file qmactivity.cpp
 * @brief QmActivity

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
#include "qmactivity.h"
#include "qmactivity_p.h"
#include "qmipcinterface.h"

#include <QDebug>

namespace MeeGo {

QmActivity::QmActivity(QObject *parent)
           : QObject(parent){
    MEEGO_INITIALIZE(QmActivity);
    connect (priv, SIGNAL(activityChanged(MeeGo::QmActivity::Activity)),
             this, SIGNAL(activityChanged(MeeGo::QmActivity::Activity)));
}

QmActivity::~QmActivity(){
    MEEGO_UNINITIALIZE(QmActivity);
}

QmActivity::Activity QmActivity::get() const{
#if HAVE_MCE
    MEEGO_PRIVATE_CONST(QmActivity)
    QmIPCInterface *requestIf = priv->requestIf;
    QList<QVariant> results;

    results = requestIf->get(MCE_INACTIVITY_STATUS_GET);
    if ( !results.empty() && !results[0].toBool() )
        return Active;
    else
        return Inactive;
#else
    return Inactive;
#endif

}

}
