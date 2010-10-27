/*!
 * @file qmusbmode_p.h
 * @brief Contains QmUSBModePrivate

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
#ifndef QMUSBMODE_P_H
#define QMUSBMODE_P_H

#include "qmusbmode.h"
#include "qmipcinterface.h"
#include <gconf/gconf-client.h>
#include <QDebug>
#include <usb-moded/usb_moded-dbus.h>
#include <usb-moded/usb_moded-modes.h>

#define USB_MODE_GCONF          "/Meego/System/UsbMode"

namespace MeeGo
{
    class QmUSBModePrivate : public QObject
    {
    public:
        Q_OBJECT;
        MEEGO_DECLARE_PUBLIC(QmUSBMode);

        QmIPCInterface *requestIf;
        GConfClient *gcClient;

        QmUSBModePrivate(QObject *parent = 0) : QObject(parent)
        {
            requestIf = new QmIPCInterface(USB_MODE_SERVICE, USB_MODE_OBJECT, USB_MODE_INTERFACE);
            requestIf->connect(USB_MODE_SIGNAL_NAME, this, SLOT(modeChanged(const QString&)));

            g_type_init();
            gcClient = gconf_client_get_default();
        }

        ~QmUSBModePrivate()
        {
            delete requestIf;
            g_object_unref(gcClient);
        }

        static QString modeToString(QmUSBMode::Mode mode)
        {
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
                qWarning() << "A bad mode: " << mode;
                return "";
            }
        }

        static QmUSBMode::Mode stringToMode(const QString &str)
        {
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
                qWarning() << "A bad mode: " << str;
                return QmUSBMode::Undefined;
            }
        }

        bool setMode(QmUSBMode::Mode mode)
        {
            QString str = modeToString(mode);
            if (str.isEmpty()) {
                return false;
            }
            return requestIf->callSynchronously(USB_MODE_STATE_SET, str);
        }

        QmUSBMode::Mode getMode()
        {
            QList<QVariant> ret = requestIf->get(USB_MODE_STATE_REQUEST);
            if (ret.size() == 1) {
                return stringToMode(ret.first().toString());
            }
            return QmUSBMode::Undefined;
        }

        bool setDefaultMode(QmUSBMode::Mode mode)
        {
            QString str = modeToString(mode);
            if (str.isEmpty()) {
                return false;
            }
            gboolean ret = gconf_client_set_string(gcClient, USB_MODE_GCONF, str.toAscii().data(), NULL);
            if (ret == TRUE) {
                return true;
            } else {
                qWarning() << "Could not set the default usb mode";
                return false;
            }
        }

        QmUSBMode::Mode getDefaultMode()
        {
            QmUSBMode::Mode mode = QmUSBMode::Undefined;
            gchar *val = gconf_client_get_string(gcClient, USB_MODE_GCONF, NULL);
            if (val) {
                mode = stringToMode(QString(val));
            } else {
                qWarning() << "Could not get the default usb mode";
            }
            g_free(val);

            return mode;
        }

    Q_SIGNALS:
        void modeChanged(MeeGo::QmUSBMode::Mode mode);

    public Q_SLOTS:
        void modeChanged(const QString &mode)
        {
            emit modeChanged(stringToMode(mode));
        }
    };
}
#endif // QMUSBMODE_P_H
