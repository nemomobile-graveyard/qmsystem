/*!
* @file qmdevicemode_p.h
* @brief Contains QmDeviceModePrivate

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
#ifndef QMDEVICEMODE_P_H
#define QMDEVICEMODE_P_H

#include "qmdevicemode.h"
#include "qmipcinterface_p.h"

#if HAVE_MCE
    #include "mce/dbus-names.h"
    #include "mce/mode-names.h"
#endif

#include <gconf/gconf-client.h>
#define PATH "/system/osso/dsm/energymanagement"
#define FORCE_POWER_SAVING PATH"/force_power_saving"
#define ENABLE_POWER_SAVING PATH"/enable_power_saving"
#define THRESHOLDS PATH"/possible_psm_thresholds"
#define THRESHOLD PATH"/psm_threshold"

// Prepare for the upcoming change in mce-dev.
#ifndef MCE_PSM_STATE_GET
#define MCE_PSM_STATE_GET MCE_PSM_MODE_GET
#endif
#ifndef MCE_PSM_STATE_SIG
#define MCE_PSM_STATE_SIG MCE_PSM_MODE_SIG
#endif

namespace MeeGo
{

    class QmDeviceModePrivate : public QObject
    {
        Q_OBJECT
        MEEGO_DECLARE_PUBLIC(QmDeviceMode)

    public:
        QmDeviceModePrivate(){
#if HAVE_MCE
            signalIf = new QmIPCInterface(
                        MCE_SERVICE,
                        MCE_SIGNAL_PATH,
                        MCE_SIGNAL_IF);
            requestIf = new QmIPCInterface(
                        MCE_SERVICE,
                        MCE_REQUEST_PATH,
                        MCE_REQUEST_IF);
#endif

            g_type_init();
            gcClient = gconf_client_get_default();

#if HAVE_MCE
            signalIf->connect(MCE_RADIO_STATES_SIG, this, SLOT(deviceModeChangedSlot(const unsigned int)));
            signalIf->connect(MCE_PSM_STATE_SIG, this, SLOT(devicePSMChangedSlot(bool)));
#endif
        }

        ~QmDeviceModePrivate(){
#if HAVE_MCE
            delete requestIf;
            delete signalIf;
#endif
            g_object_unref(gcClient);
        }

        QmIPCInterface *requestIf;
        QmIPCInterface *signalIf;
        GConfClient *gcClient;

    Q_SIGNALS:

        void devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState);
        void deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode);

    private Q_SLOTS:
        void devicePSMChangedSlot(bool on) {
            if (on) {
                emit devicePSMStateChanged(QmDeviceMode::PSMStateOn);
            } else {
                emit devicePSMStateChanged(QmDeviceMode::PSMStateOff);
            }
        }
        void deviceModeChangedSlot(const unsigned int state) {
            if (state != 0) {
                emit deviceModeChanged(QmDeviceMode::Normal);
            } else if (state == 0) {
                emit deviceModeChanged(QmDeviceMode::Flight);
            }
        }
    };

}

#endif // QMDEVICEMODE_P_H
