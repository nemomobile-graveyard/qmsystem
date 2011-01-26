/*!
 * @file qmusbmode_p.h
 * @brief Contains QmUSBModePrivate

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Matias Muhonen <ext-matias.muhonen@nokia.com>

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
#include "qmipcinterface_p.h"
#include <gconf/gconf-client.h>

#include <QMutex>
#include <QPair>
#include <QVector>
#include <QStringList>
#include <mntent.h>

#if HAVE_USB_MODED_DEV
    #include <usb_moded-dbus.h>
    #include <usb_moded-modes.h>
#else
    /* Use QmSystem D-Bus i/f declarations because usb-moded-dev is not available */
    #include "msystemdbus_p.h"
#endif

#define USB_MODE_GCONF          "/Meego/System/UsbMode"

#define SIGNAL_USB_MODE 0
#define SIGNAL_USB_ERROR 1

namespace MeeGo
{

class QmUSBModePrivate : public QObject
{
public:
    Q_OBJECT;
    MEEGO_DECLARE_PUBLIC(QmUSBMode);

    QmIPCInterface *requestIf;
    GConfClient *gcClient;
    QMutex connectMutex;
    size_t connectCount[2];

    QmUSBModePrivate(QObject *parent = 0);
    ~QmUSBModePrivate();

    static QString modeToString(QmUSBMode::Mode mode);
    static QmUSBMode::Mode stringToMode(const QString &str);

    QVector< QPair< QString , QString > > mountEntries();
    QString mountOptions(QVector< QPair< QString , QString > > mountEntries, const QString &fileName);

Q_SIGNALS:
    void modeChanged(MeeGo::QmUSBMode::Mode mode);
    void error(const QString &errorCode);

public Q_SLOTS:
    void didReceiveError(const QString &errorCode);
    void modeChanged(const QString &mode);
};

} // namespace MeeGo

#endif // QMUSBMODE_P_H
