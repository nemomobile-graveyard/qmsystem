/*!
 * @file qmdevicemode.cpp
 * @brief QmDeviceMode

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
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
#include "qmdevicemode.h"
#include "qmdevicemode_p.h"


#include <QDebug>


/*

PSM stuff (spec)

Signal emitted from the com.nokia.mce.signal interface

    Name        powersave_mode_ind
    Parameters  dbus_bool_t  mode TRUE (=on)/ FALSE(=off)
    Description Sent when the powersave mode is changed


Generic method calls provided by the com.nokia.mce.request interface

    Name                   get_powersave_mode
    Parameters             -
    Errors / Return value  dbus_bool_t inactivity TRUE / FALSE
    Description            Get the current powersave mode. TRUE if the powersave mode is on,  FALSE if the powersave mode is off.


    Name                   set_powersave_mode
    Parameters             dbus_bool_t  mode TRUE (=on)/ FALSE(=off)
    Errors / Return value  dbus_bool_t  status TRUE=success / FALSE=failure
    Description            Set the current powersave mode. TRUE if the powersave mode is on,  FALSE if the powersave mode is off.


We also need to set the automatic PSM, GConf would be good as this is persistent setting (see qmdisplaystate.cpp)



*/

namespace MeeGo {


    QmDeviceMode::QmDeviceMode(QObject *parent) : QObject(parent){
        MEEGO_INITIALIZE(QmDeviceMode)
                connect(priv, SIGNAL(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState)), this, SIGNAL(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState)));
        connect(priv, SIGNAL(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode)), this, SIGNAL(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode)));

    }

    QmDeviceMode::~QmDeviceMode(){
        MEEGO_UNINITIALIZE(QmDeviceMode);
    }

    QmDeviceMode::DeviceMode QmDeviceMode::getMode() const{
#if __MCE__
        MEEGO_PRIVATE_CONST(QmDeviceMode)
        QList<QVariant> list = priv->requestIf->get(MCE_RADIO_STATES_GET);
        unsigned int state = 0;

        if (!list.isEmpty()) {
            state = list[0].toInt();
        } else return Error;
        if (state != 0) {
            return Normal;
        } else if (state == 0) {
            return Flight;
        } else {
            return Error;
        }
#else
        return Error;
#endif
    }

    QmDeviceMode::PSMState QmDeviceMode::getPSMState() const {
#if __MCE__
        MEEGO_PRIVATE_CONST(QmDeviceMode)

                QList<QVariant> list = priv->requestIf->get(MCE_PSM_STATE_GET);
        if (!list.isEmpty()) {
            if (list.first().toBool()) {
                return PSMStateOn;
            } else {
                return PSMStateOff;
            }
        }
#endif
        return PSMError;
    }


    bool QmDeviceMode::setMode(QmDeviceMode::DeviceMode mode){
#if __MCE__
        MEEGO_PRIVATE(QmDeviceMode)

        unsigned int state, mask;

        switch (mode) {
        case Normal:
            state = 1;
            mask = MCE_RADIO_STATE_MASTER;
            break;
        case Flight:
            state = 0;
            mask = MCE_RADIO_STATE_MASTER;
            break;
        default:
            return false;
        }

        QList<QVariant> res = priv->requestIf->get(MCE_RADIO_STATES_CHANGE_REQ, state, mask);
        if (!res.isEmpty() && !res.at(0).toInt()) {
            return true;
        }
#else
    Q_UNUSED(mode);
#endif
        return false;
    }

    bool QmDeviceMode::setPSMState(QmDeviceMode::PSMState state) {
        MEEGO_PRIVATE(QmDeviceMode)

                gboolean val = FALSE;
        if (state == PSMStateOff) {
            val = FALSE;
        } else if (state == PSMStateOn) {
            val = TRUE;
        } else {
            return false;
        }

        gboolean ret = gconf_client_set_bool(priv->gcClient, FORCE_POWER_SAVING, val, NULL);
        if (ret == TRUE) {
            return true;
        } else {
            return false;
        }
    }

    bool QmDeviceMode::setPSMBatteryMode(int percentages) {
        MEEGO_PRIVATE(QmDeviceMode)

                if (percentages < 0 || percentages > 100) {
            return false;
        }

        int value = 0;
        if (percentages > 0) {
            GSList *list = gconf_client_get_list(priv->gcClient, THRESHOLDS, GCONF_VALUE_INT, NULL);
            if (!list) {
                return false;
            }
            GSList *elem = list;
            do {
                int data = GPOINTER_TO_INT(elem->data);
                if (percentages <= data || !elem->next) {
                    value = data;
                    break;
                }
            } while (elem = g_slist_next(elem));
            g_slist_free(list);
        }

        gboolean ret = FALSE;
        if (value == 0) {
            ret = gconf_client_set_bool(priv->gcClient, ENABLE_POWER_SAVING, FALSE, NULL);
        } else {
            ret = gconf_client_set_bool(priv->gcClient, ENABLE_POWER_SAVING, TRUE, NULL);
            if (ret == TRUE) {
                ret = gconf_client_set_int(priv->gcClient, THRESHOLD, value, NULL);
            }
        }

        if (ret == TRUE) {
            return true;
        } else {
            return false;
        }
    }

    int QmDeviceMode::getPSMBatteryMode() {
        MEEGO_PRIVATE(QmDeviceMode)

                GError *error = NULL;
        gboolean ret = gconf_client_get_bool(priv->gcClient, ENABLE_POWER_SAVING, &error);
        if (error) {
            g_error_free(error);
            return -1;
        }
        if (ret == FALSE) {
            return 0;
        }
        int retVal = gconf_client_get_int(priv->gcClient, THRESHOLD, &error);
        if (error) {
            g_error_free(error);
            return -1;
        }
        return retVal;
    }


} // namespace MeeGo
