/*!
 * @file qmusbmode.cpp
 * @brief QmUSBMode

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
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
#include "qmusbmode.h"
#include "qmusbmode_p.h"

namespace MeeGo {

QmUSBModePrivate::QmUSBModePrivate(QObject *parent) : QObject(parent)
{
    requestIf = new QmIPCInterface(USB_MODE_SERVICE, USB_MODE_OBJECT, USB_MODE_INTERFACE);

    connectCount[SIGNAL_USB_MODE] = connectCount[SIGNAL_USB_ERROR] = 0;

    g_type_init();
    gcClient = gconf_client_get_default();
}

QmUSBModePrivate::~QmUSBModePrivate() {
    if (requestIf) {
        delete requestIf, requestIf = 0;
    }
    g_object_unref(gcClient), gcClient = 0;
}

QString QmUSBModePrivate::modeToString(QmUSBMode::Mode mode) {
    switch (mode) {
    case QmUSBMode::Connected:
        return USB_CONNECTED;
    case QmUSBMode::DataInUse:
        return DATA_IN_USE;
    case QmUSBMode::Disconnected:
        return USB_DISCONNECTED;
    case QmUSBMode::MassStorage:
        return MODE_MASS_STORAGE;
    case QmUSBMode::ChargingOnly:
        return MODE_CHARGING;
    case QmUSBMode::OviSuite:
        return MODE_OVI_SUITE;
    case QmUSBMode::Ask:
        return MODE_ASK;
    case QmUSBMode::Undefined:
        return MODE_UNDEFINED;
    case QmUSBMode::ModeRequest:
        return USB_CONNECTED_DIALOG_SHOW;
    default:
        return "";
    }
}

QmUSBMode::Mode QmUSBModePrivate::stringToMode(const QString &str) {
    if (str == USB_CONNECTED) {
        return QmUSBMode::Connected;
    } else if (str == USB_DISCONNECTED) {
        return QmUSBMode::Disconnected;
    } else if (str == DATA_IN_USE) {
        return QmUSBMode::DataInUse;
    } else if (str == MODE_MASS_STORAGE) {
        return QmUSBMode::MassStorage;
    } else if (str == MODE_OVI_SUITE) {
        return QmUSBMode::OviSuite;
    } else if (str == MODE_CHARGING) {
        return QmUSBMode::ChargingOnly;
    } else if (str == MODE_ASK) {
        return QmUSBMode::Ask;
    } else if (str == MODE_UNDEFINED) {
        return QmUSBMode::Undefined;
    } else if (str == USB_CONNECTED_DIALOG_SHOW) {
        return QmUSBMode::ModeRequest;
    } else {
        return QmUSBMode::Undefined;
    }
}

bool QmUSBModePrivate::setMode(QmUSBMode::Mode mode) {
    QString str = modeToString(mode);
    if (str.isEmpty()) {
        return false;
    }
    requestIf->callAsynchronously(USB_MODE_STATE_SET, str);
    return true;
}

QmUSBMode::Mode QmUSBModePrivate::getMode() {
    QList<QVariant> ret = requestIf->get(USB_MODE_STATE_REQUEST);
    if (ret.size() == 1) {
        return stringToMode(ret.first().toString());
    }
    return QmUSBMode::Undefined;
}

bool QmUSBModePrivate::setDefaultMode(QmUSBMode::Mode mode) {
    QString str = modeToString(mode);
    if (str.isEmpty()) {
        return false;
    }
    gboolean ret = gconf_client_set_string(gcClient, USB_MODE_GCONF, str.toAscii().data(), NULL);
    if (ret == TRUE) {
        return true;
    } else {
        return false;
    }
}

QmUSBMode::Mode QmUSBModePrivate::getDefaultMode() {
    QmUSBMode::Mode mode = QmUSBMode::Undefined;
    gchar *val = gconf_client_get_string(gcClient, USB_MODE_GCONF, NULL);
    if (val) {
        mode = stringToMode(QString(val));
    }
    g_free(val);
    return mode;
}

void QmUSBModePrivate::didReceiveError(const QString &errorCode) {
    emit error(errorCode);
}

void QmUSBModePrivate::modeChanged(const QString &mode) {
    emit modeChanged(stringToMode(mode));
}

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

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode))))) {
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

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode))))) {
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

QmUSBMode::MountOptionFlags QmUSBMode::mountStatus(QmUSBMode::MountPath mounthPath) {
    QmUSBMode::MountOptionFlags mountOptionFlags;
    return mountOptionFlags;
}

} // namespace MeeGo
