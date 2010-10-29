/*!
 * @file msystemdbus.h
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

//TODO
//Move this file to msystemdbus_p.h

#ifndef MSYSTEMDBUS_H
#define MSYSTEMDBUS_H

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
#define SYS_DSME_REQ_DENIED_SIG       "denied_req_ind"

#define SYS_DSME_POWERUP_REQ          "req_powerup"
#define SYS_DSME_REBOOT_REQ           "req_reboot"
#define SYS_DSME_SHUTDOWN_REQ         "req_shutdown"

#define SYS_DSME_VERSION_GET          "get_version"

// (MCE)
#define SYS_MCE_SERVICE               "com.nokia.mce"

#define SYS_MCE_SIG_INTERFACE         "com.nokia.mce.signal"
#define SYS_MCE_SIG_PATH              "/com/nokia/mce/signal"

#define SYS_MCE_REQ_INTERFACE         "com.nokia.mce.request"
#define SYS_MCE_REQ_PATH              "/com/nokia/mce/request"

#define SYS_MCE_VERSION_GET           "get_version"

#define SYS_MCE_DEVICEMODE_GET        "get_device_mode"
#define SYS_MCE_DEVICEMODE_REQ        "req_device_mode_change"
#define SYS_MCE_DEVICEMODE_SIG        "sig_device_mode_ind"
#define SYS_MCE_PSM_GET               "get_psm_mode"
#define SYS_MCE_PSM_SIG               "psm_mode_ind"

//ACTIVITY STATUS (MCE)
#define SYS_MCE_INACTIVITY_SIG        "system_inactivity_ind"

#define SYS_MCE_INACTIVITY_GET        "get_inactivity_status"

//DISPLAY STATUS (MCE)
#define SYS_MCE_DISPLAYSTATUS_GET         "get_display_status"

#define SYS_MCE_DISPLAYBLANKINGPAUSE_REQ  "req_display_blanking_pause"
#define SYS_MCE_DISPLAYSTATEOFF_REQ       "req_display_state_off"
#define SYS_MCE_DISPLAYSTATEDIM_REQ       "req_display_state_dim"
#define SYS_MCE_DISPLAYSTATEON_REQ        "req_display_state_on"

#define SYS_MCE_DISPLAYSTATUS_SIG         "display_status_ind"

//LOCK (MCE)
#define SYS_MCE_TKMODE_GET                "get_tklock_mode"
#define SYS_MCE_DEVICELOCKMODE_GET        "get_devicelock_mode"

#define SYS_MCE_TSKBMODE_REQ              "req_tklock_mode_change"
#define SYS_MCE_DEVICELOCKMODE_REQ        "req_devlock_mode_change"

#define SYS_MCE_DEVICELOCKMODE_SIG        "devicelock_mode_ind"
#define SYS_MCE_TKMODE_SIG                "tklock_mode_ind"


//CALLSTATE (MCE)
#define SYS_MCE_CALLSTATE_GET             "get_call_state"
#define SYS_MCE_CALLSTATE_REQ             "req_call_state_change"
#define SYS_MCE_CALLSTATE_SIG             "sig_call_state_ind"

//LED (MCE)
#define SYS_MCE_LED_PATTERNACTIVATE_REQ    "req_led_pattern_activate"
#define SYS_MCE_LED_PATTERNDEACTIVATE_REQ  "req_led_pattern_deactivate"
#define SYS_MCE_LED_ENABLE_REQ             "req_led_enable"
#define SYS_MCE_LED_DISABLE_REQ            "req_led_disable"

//POWERKEY (MCE)
#define SYS_MCE_POWERKEY_EVENT_REQ          "req_trigger_powerkey_event"

//HOMEKEY (MCE)
#define SYS_MCE_HOMEKEY_SIG                "sig_home_key_pressed_ind"
#define SYS_MCE_HOMEKEY_LONG_SIG           "sig_home_key_pressed_long_ind"


//THERMAL
#define SYS_THERMALMANAGER_SERVICE        "com.nokia.thermalmanager"
#define SYS_THERMALMANAGER_INTERFACE      "com.nokia.thermalmanager"
#define SYS_THERMALMANAGER_PATH           "/com/nokia/thermalmanager"

#define SYS_THERMALMANAGER_STATE_GET      "get_thermal_state"
#define SYS_THERMALMANAGER_STATE_SIG      "thermal_state_change_ind"

//BATTERY (BME via HAL)
#define SYS_HAL_DEVICE_SERVICE            "org.freedesktop.Hal"
#define SYS_HAL_DEVICE_INTERFACE          "org.freedesktop.Hal.Device"

#define SYS_HAL_DEVICE_BME_PATH           "/org/freedesktop/Hal/devices/bme"

#define SYS_HAL_PROPERTY_GET              "GetProperty"
#define SYS_HAL_PROPERTY_SET              "SetProperty"
#define SYS_HAL_PROPERTY_SIG              "PropertyModified"

//BATTERY (BME)
#define SYS_BME_INTERFACE                 "com.nokia.bme"
#define SYS_BME_SIGNAL_INTERFACE          "com.nokia.bme.signal"
#define SYS_BME_SIGNAL_PATH               "/com/nokia/bme/signal"

#define SYS_BME_BATTERY_FULL                "battery_full"
#define SYS_BME_BATTERY_LOW                 "battery_low"
#define SYS_BME_BATTERY_EMPTY               "battery_empty"

#define SYS_BME_CHARGER_CONNECTED           "charger_connected"
#define SYS_BME_CHARGER_DISCONNECTED        "charger_disconnected"

#define SYS_BME_CHARGER_CHARGING_ON         "charger_charging_on"
#define SYS_BME_CHARGER_CHARGING_OFF        "charger_charging_off"
#define SYS_BME_CHARGER_CHARGING_FAILED     "charger_charging_failed"

#define SYS_BME_BATTERY_STATE_CHANGED       "battery_state_changed"

//ALARM
#define SYS_ALARM_SERVICE                 "com.nokia.alarmd"
#define SYS_ALARM_INTERFACE               "com.nokia.alarmd"
#define SYS_ALARM_PATH                    "/com/nokia/alarmd"

#define SYS_ALARM_QUEUE_SIG               "queue_status_ind"

//CLOCK
#define SYS_CLOCK_SERVICE                 "com.nokia.clockd"
#define SYS_CLOCK_INTERFACE               "com.nokia.clockd"
#define SYS_CLOCK_PATH                    "/com/nokia/clockd"

#define SYS_CLOCK_TIME_SIG                "time_changed"

#endif // MSYSTEMDBUS_H
