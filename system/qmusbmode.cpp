/*!
 * @file qmusbmode.cpp
 * @brief QmUSBMode

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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
#include "qmusbmode.h"
#include "qmusbmode_p.h"

namespace MeeGo {

    QmUSBMode::QmUSBMode(QObject *parent) : QObject(parent) {
        MEEGO_INITIALIZE(QmUSBMode);
        connect(priv, SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode)), this, SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode)));
        connect(priv, SIGNAL(error(const QString&)), this, SIGNAL(error(const QString&)));
    }

    QmUSBMode::~QmUSBMode() {
        MEEGO_PRIVATE(QmUSBMode);

        disconnect(priv, SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode)), this, SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode)));
        disconnect(priv, SIGNAL(error(const QString&)), this, SIGNAL(error(const QString&)));

        MEEGO_UNINITIALIZE(QmUSBMode);
    }

    void QmUSBMode::connectNotify(const char *signal) {
        MEEGO_PRIVATE(QmUSBMode)

        /* QObject::connect() needs to be thread-safe */
        QMutexLocker locker(&priv->connectMutex);

        if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode mode))))) {
            if (0 == priv->connectCount[SIGNAL_USB_MODE]) {
                QDBusConnection::systemBus().connect(USB_MODE_SERVICE,
                                                     USB_MODE_OBJECT,
                                                     USB_MODE_INTERFACE,
                                                     USB_MODE_SIGNAL_NAME,
                                                     priv,
                                                     SLOT(modeChanged(const QString&)));
            }
            priv->connectCount[SIGNAL_USB_MODE]++;
        } else if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(error(const QString&))))) {
            if (0 == priv->connectCount[SIGNAL_USB_ERROR]) {
                QDBusConnection::systemBus().connect(USB_MODE_SERVICE,
                                                     USB_MODE_OBJECT,
                                                     USB_MODE_INTERFACE,
                                                     USB_MODE_ERROR_SIGNAL_NAME,
                                                     priv,
                                                     SLOT(didReceiveError(const QString&)));
            }
            priv->connectCount[SIGNAL_USB_ERROR]++;
        }
    }

    void QmUSBMode::disconnectNotify(const char *signal) {
        MEEGO_PRIVATE(QmUSBMode)

        /* QObject::disconnect() needs to be thread-safe */
        QMutexLocker locker(&priv->connectMutex);

        if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode mode))))) {
            priv->connectCount[SIGNAL_USB_MODE]--;

            if (0 == priv->connectCount[SIGNAL_USB_MODE]) {
                QDBusConnection::systemBus().disconnect(USB_MODE_SERVICE,
                                                        USB_MODE_OBJECT,
                                                        USB_MODE_INTERFACE,
                                                        USB_MODE_SIGNAL_NAME,
                                                        priv,
                                                        SLOT(modeChanged(const QString&)));
            }
        } else if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(error(const QString&))))) {
            priv->connectCount[SIGNAL_USB_ERROR]--;

            if (0 == priv->connectCount[SIGNAL_USB_ERROR]) {
                QDBusConnection::systemBus().disconnect(USB_MODE_SERVICE,
                                                        USB_MODE_OBJECT,
                                                        USB_MODE_INTERFACE,
                                                        USB_MODE_ERROR_SIGNAL_NAME,
                                                        priv,
                                                        SLOT(didReceiveError(const QString&)));
            }
        }
    }

    QmUSBMode::Mode QmUSBMode::getMode() {
        MEEGO_PRIVATE(QmUSBMode);
        return priv->getMode();
    }

    bool QmUSBMode::setMode(QmUSBMode::Mode mode) {
        MEEGO_PRIVATE(QmUSBMode);
        switch (mode) {
        case OviSuite:
        case MassStorage:
        case ChargingOnly:
            return priv->setMode(mode);
        default:
            return false;
        }
    }

    bool QmUSBMode::setDefaultMode(QmUSBMode::Mode mode) {
        MEEGO_PRIVATE(QmUSBMode);
        switch (mode) {
        case OviSuite:
        case MassStorage:
        case ChargingOnly:
        case Ask:
            return priv->setDefaultMode(mode);
        default:
            return false;

        }
    }

    QmUSBMode::Mode QmUSBMode::getDefaultMode() {
        MEEGO_PRIVATE(QmUSBMode);
        return priv->getDefaultMode();
    }
}
