/*!
* @file qmactivity_p.h
* @brief Contains QmActivityPrivate

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Timo Rongas <ext-timo.rongas.nokia.com>
   @author Antonio Aloisio <antonio.aloisio@nokia.com>

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
#ifndef QMACTIVITY_P_H
#define QMACTIVITY_P_H

#include "qmactivity.h"
#include "qmipcinterface.h"

#if __MCE__
    #include "mce/dbus-names.h"
    #include "mce/mode-names.h"
#endif

namespace MeeGo
{

    class QmActivityPrivate : public QObject
    {
        Q_OBJECT
        MEEGO_DECLARE_PUBLIC(QmActivity)

    public:
        QmIPCInterface *requestIf;
        QmIPCInterface *signalIf;

        QmActivityPrivate(){
#if __MCE__
            signalIf = new QmIPCInterface(
                        MCE_SERVICE,
                        MCE_SIGNAL_PATH,
                        MCE_SIGNAL_IF);
            requestIf = new QmIPCInterface(
                        MCE_SERVICE,
                        MCE_REQUEST_PATH,
                        MCE_REQUEST_IF);
            signalIf->connect(MCE_INACTIVITY_SIG, this, SLOT(slotActivityChanged(bool)));
#endif
        }

        ~QmActivityPrivate(){
#if __MCE__
            delete requestIf;
            delete signalIf;
#endif
        }

    Q_SIGNALS:
        void activityChanged(MeeGo::QmActivity::Activity);

    public Q_SLOTS:
        void slotActivityChanged(bool inactivity){
            if (inactivity) {
                emit activityChanged(QmActivity::Inactive);
            } else {
                emit activityChanged(QmActivity::Active);
            }
        }

    };

}

#endif // QMACTIVITY_P_H
