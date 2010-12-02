/*!
 * @file msystemdbus_p.h
 * @brief Contains D-Bus names for the services accessible through this API.

   <p>
   @copyright (C) 2009-2010 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.rongas.nokia.com>
   @author Tuomo Tanskanen <ext-tuomo.1.tanskanen@nokia.com>

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
 *
 */

#ifndef MSYSTEMDBUS_P_H
#define MSYSTEMDBUS_P_H

//D-Bus interfaces

//Device State Management Entity (DSME)
#define SYS_DSME_SERVICE              "com.nokia.dsme"

#define SYS_DSME_SIG_INTERFACE        "com.nokia.dsme.signal"
#define SYS_DSME_SIG_PATH             "/com/nokia/dsme/signal"

#define SYS_DSME_REQ_INTERFACE        "com.nokia.dsme.request"
#define SYS_DSME_REQ_PATH             "/com/nokia/dsme/request"

#define SYS_DSME_SHUTDOWN_SIG         "shutdown_ind"
#define SYS_DSME_THERMALSHUTDOWN_SIG  "thermal_shutdown_ind"
#define SYS_DSME_UNSAVEDDATA_SIG      "save_unsaved_data_ind"
#define SYS_DSME_BATTERYEMPTY_SIG     "battery_empty_ind"

#define SYS_DSME_POWERUP_REQ          "req_powerup"
#define SYS_DSME_REBOOT_REQ           "req_reboot"
#define SYS_DSME_SHUTDOWN_REQ         "req_shutdown"

//THERMAL
#define SYS_THERMALMANAGER_SERVICE        "com.nokia.thermalmanager"
#define SYS_THERMALMANAGER_INTERFACE      "com.nokia.thermalmanager"
#define SYS_THERMALMANAGER_PATH           "/com/nokia/thermalmanager"

#define SYS_THERMALMANAGER_STATE_GET      "get_thermal_state"
#define SYS_THERMALMANAGER_STATE_SIG      "thermal_state_change_ind"

//POWERONTIMER
#define SYS_POWERONTIMER_SERVICE          "com.nokia.powerontimer"
#define SYS_POWERONTIMER_INTERFACE        "com.nokia.powerontimer"
#define SYS_POWERONTIMER_PATH             "/com/nokia/powerontimer"

#define SYS_POWERONTIMER_TIME_GET         "get_poweron_time"

#endif // MSYSTEMDBUS_P_H
