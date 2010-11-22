/*!
 * @file qmsystemstate.cpp
 * @brief QmSystemState

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Tuomo Tanskanen <ext-tuomo.1.tanskanen@nokia.com>
   @author Simo Piiroinen <simo.piiroinen@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>

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
#include "qmsystemstate.h"
#include "qmsystemstate_p.h"

#include <stdlib.h> /* free() */
#include <sysinfo.h>

#include <QFile>
#include <QTextStream>

#define RUNSTATE_FILE "/var/lib/dsme/saved_state"

#define RUNSTATE_LOCAL "LOCAL"
#define RUNSTATE_TEST "TEST"
#define RUNSTATE_MALF "MALF"
#define RUNSTATE_FLASH "FLASH"
#define RUNSTATE_USER "USER"
#define RUNSTATE_SHUTDOWN "SHUTDOWN"
#define RUNSTATE_ACT_DEAD "ACT_DEAD"

#define SYSINFO_KEY_BOOTREASON "/component/boot-reason"

#define BOOT_REASON_UNKNOWN           "unknown"
#define BOOT_REASON_SWDG_TIMEOUT     "swdg_to"
#define BOOT_REASON_SEC_VIOLATION    "sec_vio"
#define BOOT_REASON_32K_WDG_TIMEOUT  "32wd_to"
#define BOOT_REASON_POWER_ON_RESET   "por"
#define BOOT_REASON_POWER_KEY        "pwr_key"
#define BOOT_REASON_MBUS             "mbus"
#define BOOT_REASON_CHARGER          "charger"
#define BOOT_REASON_USB              "usb"
#define BOOT_REASON_SW_RESET         "sw_rst"
#define BOOT_REASON_RTC_ALARM        "rtc_alarm"
#define BOOT_REASON_NSU              "nsu"

namespace MeeGo {

QmSystemState::QmSystemState(QObject *parent)
             : QObject(parent){
    MEEGO_INITIALIZE(QmSystemState);
    connect(priv, SIGNAL(systemStateChanged(MeeGo::QmSystemState::StateIndication)), this, SIGNAL(systemStateChanged(MeeGo::QmSystemState::StateIndication)));
}

QmSystemState::~QmSystemState(){
    MEEGO_UNINITIALIZE(QmSystemState);
}

bool QmSystemState::set(NextState nextState){
    MEEGO_PRIVATE(QmSystemState)

    QString method;
    switch (nextState){
        case Powerup:
            method = QString(dsme_req_powerup);
            break;
        case Reboot:
            method = QString(dsme_req_reboot);
            break;
        case ShuttingDown:
            method = QString(dsme_req_shutdown);
            break;
        default:
            return false;
    }
    QmIPCInterface *requestIf = priv->dsmeRequestIf;
    return requestIf->callAsynchronously(method);
}

QmSystemState::RunState QmSystemState::getRunState() {
    QFile file(RUNSTATE_FILE);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString stateStr = in.readLine();
        if (stateStr == RUNSTATE_LOCAL) {
            return RunState_Local;
        } else if (stateStr == RUNSTATE_TEST) {
            return RunState_Test;
        } else if (stateStr == RUNSTATE_MALF) {
            return RunState_Malf;
        } else if (stateStr == RUNSTATE_FLASH) {
            return RunState_Flash;
        } else if (stateStr == RUNSTATE_USER) {
            return RunState_User;
        } else if (stateStr == RUNSTATE_SHUTDOWN) {
            return RunState_Shutdown;
        } else if (stateStr == RUNSTATE_ACT_DEAD) {
            return RunState_ActDead;
        }
    }

    return RunState_Unknown;
}

QmSystemState::BootReason QmSystemState::getBootReason() {
    QmSystemState::BootReason bootReason = BootReason_Unknown;
    struct system_config *sc = 0;
    uint8_t *data = 0;
    unsigned long size = 0;
    QString reasonStr("");

    if (sysinfo_init(&sc) != 0) {
        /* Failed to initialize system configuration object */
        goto BOOTREASON_DETERMINED;
    }

    if (sysinfo_get_value(sc, SYSINFO_KEY_BOOTREASON, &data, &size) != 0) {
        /* Failed to read boot-reason from system configuration */
        goto BOOTREASON_DETERMINED;
    }

    for (unsigned long k=0; k < size; k++) {
        /* Values can contain non-ascii data -> escape those */
        int c = data[k];
        if (c < 32 || c > 126)
            continue;
        reasonStr.append(QChar(c));
    }

    if (reasonStr == BOOT_REASON_SWDG_TIMEOUT) {
        bootReason = BootReason_SwdgTimeout;
    } else if (reasonStr == BOOT_REASON_SEC_VIOLATION) {
        bootReason = BootReason_SecViolation;
    } else if (reasonStr == BOOT_REASON_32K_WDG_TIMEOUT) {
        bootReason = BootReason_Wdg32kTimeout;
    } else if (reasonStr == BOOT_REASON_POWER_ON_RESET) {
        bootReason = BootReason_PowerOnReset;
    } else if (reasonStr == BOOT_REASON_POWER_KEY) {
        bootReason = BootReason_PowerKey;
    } else if (reasonStr == BOOT_REASON_MBUS) {
        bootReason = BootReason_MBus;
    } else if (reasonStr == BOOT_REASON_CHARGER) {
        bootReason = BootReason_Charger;
    } else if (reasonStr == BOOT_REASON_USB) {
        bootReason = BootReason_Usb;
    } else if (reasonStr == BOOT_REASON_SW_RESET) {
        bootReason = BootReason_SWReset;
    } else if (reasonStr == BOOT_REASON_RTC_ALARM) {
        bootReason = BootReason_RTCAlarm;
    } else if (reasonStr == BOOT_REASON_NSU) {
        bootReason = BootReason_NSU;
    }

BOOTREASON_DETERMINED:
    if (data) {
        free(data), data = 0;
    }

    if (sc) {
        sysinfo_finish(sc), sc = 0;
    }
    return bootReason;
}

unsigned int QmSystemState::getPowerOnTimeInSeconds(){
    unsigned int result = 0;
    MEEGO_PRIVATE(QmSystemState)
    QmIPCInterface *requestIf = priv->poweronRequestIf;
    QList<QVariant> results = requestIf->get(SYS_POWERONTIMER_TIME_GET);
    if( ! results.isEmpty() ) {
      result = results.first().toUInt();
    }
    return result;
}

} // MeeGo namespace
