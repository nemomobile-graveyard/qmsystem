/*!
 * @file qmthermal_p.h
 * @brief Contains QmThermalPrivate

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
#ifndef QMTHERMAL_P_H
#define QMTHERMAL_P_H

#include "qmthermal.h"

#include "qmipcinterface.h"

#define SYS_THERMALMANAGER_SERVICE        "com.nokia.thermalmanager"
#define SYS_THERMALMANAGER_INTERFACE      "com.nokia.thermalmanager"
#define SYS_THERMALMANAGER_PATH           "/com/nokia/thermalmanager"
#define SYS_THERMALMANAGER_STATE_GET      "get_thermal_state"
#define SYS_THERMALMANAGER_STATE_SIG      "thermal_state_change_ind"
#define NORMAL  "normal"
#define ALERT   "alert"
#define WARNING "warning"

namespace MeeGo
{

    class QmThermalPrivate : public QObject
    {
        Q_OBJECT
        MEEGO_DECLARE_PUBLIC(QmThermal)

    public:
        QmThermalPrivate(){
            If = new QmIPCInterface(
                           SYS_THERMALMANAGER_SERVICE,
                           SYS_THERMALMANAGER_PATH,
                           SYS_THERMALMANAGER_INTERFACE);
            If->connect(SYS_THERMALMANAGER_STATE_SIG, this, SLOT(thermalStateChanged(const QString&)));

        }

        ~QmThermalPrivate(){
            delete If;
        }

        static QmThermal::ThermalState stringToState(const QString& state){
            QmThermal::ThermalState mState = QmThermal::Unknown;

            if (state.isEmpty()){
                qWarning("Unable to get a string value for thermal state");
            }else if (state == NORMAL)
                mState = QmThermal::Normal;
            else if (state == WARNING)
                mState = QmThermal::Warning;
            else if (state == ALERT)
                mState = QmThermal::Alert;
            else
                qWarning("Unable to get a valid string value");

            return mState;
        }

        QmIPCInterface *If;

    Q_SIGNALS:
        void thermalChanged(MeeGo::QmThermal::ThermalState);

    private Q_SLOTS:
        void thermalStateChanged(const QString &state) {
            emit thermalChanged(QmThermalPrivate::stringToState(state));

        }
    };

}
#endif // QMTHERMAL_P_H
