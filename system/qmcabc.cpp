/*!
 * @file qmcabc.cpp
 * @brief QmCABC

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Tuomo Tanskanen <ext-tuomo.1.tanskanen@nokia.com>
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
#include "qmcabc.h"

#include "qmsysteminformation.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>

#if HAVE_MCE

#include <mce/mode-names.h>
#include <mce/dbus-names.h>

static bool cabcModeToString(const MeeGo::QmCABC::Mode& mode, QString& cabcString)
{
    bool success = true;

    if (MeeGo::QmCABC::Off == mode) {
        cabcString = MCE_CABC_MODE_OFF;
    } else if (MeeGo::QmCABC::Ui == mode) {
        cabcString = MCE_CABC_MODE_UI;
    } else if (MeeGo::QmCABC::StillImage == mode) {
        cabcString = MCE_CABC_MODE_STILL_IMAGE;
    } else if (MeeGo::QmCABC::MovingImage == mode) {
        cabcString = MCE_CABC_MODE_MOVING_IMAGE;
    } else {
        success = false;
    }

    return success;
}

static bool cabcStringToMode(const QString& cabcString, MeeGo::QmCABC::Mode& mode)
{
    bool success = true;

    if (MCE_CABC_MODE_OFF == cabcString) {
        mode = MeeGo::QmCABC::Off;
    } else if (MCE_CABC_MODE_UI == cabcString) {
        mode = MeeGo::QmCABC::Ui;
    } else if (MCE_CABC_MODE_STILL_IMAGE == cabcString) {
        mode = MeeGo::QmCABC::StillImage;
    } else if (MCE_CABC_MODE_MOVING_IMAGE == cabcString) {
        mode = MeeGo::QmCABC::MovingImage;
    } else {
        success = false;
    }

    return success;
}

#endif /* HAVE_MCE */

namespace MeeGo
{

bool QmCABC::set(Mode mode)
{
    bool success = false;

#if HAVE_MCE
    MeeGo::QmSystemInformation systemInformation;
    QString product;
    QString cabcString;

    if (!cabcModeToString(mode, cabcString)) {
        // Invalid mode requested
        return success;
    }

    product = systemInformation.valueForKey("/component/product");

    if ("RM-696" == product) {
        // CABC not supported
        return success;
    }

    QDBusMessage cabcModeSetCall = QDBusMessage::createMethodCall(MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF,
                                                                  MCE_CABC_MODE_REQ);
    cabcModeSetCall << cabcString;

    (void)QDBusConnection::systemBus().call(cabcModeSetCall, QDBus::NoBlock);
    success = true;
#endif /* HAVE_MCE */

    return success;
}

QmCABC::Mode QmCABC::get() const
{
    QmCABC::Mode mode = Off;

#if HAVE_MCE
    QDBusReply<QString> reply = QDBusConnection::systemBus().call(
                                    QDBusMessage::createMethodCall(MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF,
                                                                   MCE_CABC_MODE_GET));
    if (!reply.isValid()) {
        return mode;
    }

    (void)cabcStringToMode(reply.value(), mode);
#endif /* HAVE_MCE */

    return mode;
}

}  //MeeGo namespace
