/*!
 * @file qmsystemstate_p.h
 * @brief Contains QmSystemStatePrivate

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
#ifndef QMSYSTEMSTATE_P_H
#define QMSYSTEMSTATE_P_H

#include "qmsystemstate.h"
#include "qmipcinterface.h"
#if 1
#include "dsme/dsme_dbus_if.h"

#else
//TODO: ask Semi about these
#include "msystemdbus.h"
#define dsme_service SYS_DSME_SERVICE
#define dsme_sig_interface SYS_DSME_SIG_INTERFACE
#define dsme_sig_path SYS_DSME_SIG_PATH

#define dsme_req_interface SYS_DSME_REQ_INTERFACE
#define dsme_req_path SYS_DSME_REQ_PATH

#define dsme_shutdown_ind SYS_DSME_SHUTDOWN_SIG
#define dsme_save_unsaved_data_ind SYS_DSME_UNSAVEDDATA_SIG
#define dsme_battery_empty_ind SYS_DSME_BATTERYEMPTY_SIG
#define dsme_state_req_denied_ind SYS_DSME_REQ_DENIED_SIG

#define dsme_req_powerup SYS_DSME_POWERUP_REQ
#define dsme_req_reboot SYS_DSME_REBOOT_REQ
#define dsme_req_shutdown SYS_DSME_SHUTDOWN_REQ
#endif
#define SYS_THERMALMANAGER_SERVICE        "com.nokia.thermalmanager"
#define SYS_THERMALMANAGER_INTERFACE      "com.nokia.thermalmanager"
#define SYS_THERMALMANAGER_PATH           "/com/nokia/thermalmanager"

#define SYS_THERMALMANAGER_STATE_GET      "get_thermal_state"
#define SYS_THERMALMANAGER_STATE_SIG      "thermal_state_change_ind"

#define SYS_POWERONTIMER_SERVICE          "com.nokia.powerontimer"
#define SYS_POWERONTIMER_INTERFACE        "com.nokia.powerontimer"
#define SYS_POWERONTIMER_PATH             "/com/nokia/powerontimer"

#define SYS_POWERONTIMER_TIME_GET         "get_poweron_time"

namespace MeeGo
{

    class QmSystemStatePrivate : public QObject
    {
        Q_OBJECT
        MEEGO_DECLARE_PUBLIC(QmSystemState)

    public:
        QmSystemStatePrivate(){
            dsmeSignalIf = new QmIPCInterface(
                           dsme_service,
                           dsme_sig_path,
                           dsme_sig_interface);
            dsmeRequestIf = new QmIPCInterface(
                           dsme_service,
                           dsme_req_path,
                           dsme_req_interface);
            thermalSignalIf = new QmIPCInterface(
                           SYS_THERMALMANAGER_SERVICE,
                           SYS_THERMALMANAGER_PATH,
                           SYS_THERMALMANAGER_INTERFACE);
            poweronRequestIf = new QmIPCInterface(
                           SYS_POWERONTIMER_SERVICE,
                           SYS_POWERONTIMER_PATH,
                           SYS_POWERONTIMER_INTERFACE);

            dsmeSignalIf->connect(dsme_shutdown_ind, this, SLOT(emitShutdown()));
            dsmeSignalIf->connect(dsme_save_unsaved_data_ind, this, SLOT(emitSaveData()));
            dsmeSignalIf->connect(dsme_battery_empty_ind, this, SLOT(emitBatteryShutdown()));
            dsmeSignalIf->connect(dsme_state_req_denied_ind, this, SLOT(emitShutdownDenied(QString, QString)));
            thermalSignalIf->connect(SYS_THERMALMANAGER_STATE_SIG, this, SLOT(emitThermalShutdown(QString)));
        }

        ~QmSystemStatePrivate(){
            delete dsmeSignalIf;
            delete dsmeRequestIf;
            delete thermalSignalIf;
            delete poweronRequestIf;
        }

        QmIPCInterface *dsmeSignalIf;
        QmIPCInterface *dsmeRequestIf;
        QmIPCInterface *thermalSignalIf;
        QmIPCInterface *poweronRequestIf;

    Q_SIGNALS:

        void systemStateChanged(MeeGo::QmSystemState::StateIndication what);

    private Q_SLOTS:

        void emitShutdown() {          
            emit systemStateChanged(QmSystemState::Shutdown);
        }

        void emitThermalShutdown(QString thermalState) {
            // TODO: hardcoded "fatal"
            if (thermalState == "fatal") {
                emit systemStateChanged(QmSystemState::ThermalStateFatal);
            }
        }

        void emitBatteryShutdown() {
            emit systemStateChanged(QmSystemState::BatteryStateEmpty);
        }

        void emitSaveData() {
            emit systemStateChanged(QmSystemState::SaveData);
        }

        void emitShutdownDenied(QString reqType, QString reason) {
            if (reason == "usb") {
                if (reqType == "shutdown") {
                    emit systemStateChanged(QmSystemState::ShutdownDeniedUSB);
                } else if (reqType == "reboot") {
                    emit systemStateChanged(QmSystemState::RebootDeniedUSB);
                }
            }
        }
    };

}
#endif // QMSYSTEMSTATE_P_H
