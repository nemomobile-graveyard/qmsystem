/*!
 * @file qmipcinterface.h
 * @brief Contains QmIPCInterface and QmIPCInterfaceInfo.

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.rongas.nokia.com>

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
#ifndef QMIPCINTERFACE_H
#define QMIPCINTERFACE_H

#include "system_global.h"

#include <QtDBus/qdbusconnection.h>
#include <QtDBus/qdbusinterface.h>
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <QtCore/qvariant.h>

class QDBusConnection;

namespace MeeGo {

class QmIPCInterfaceInfo
{
public:
    QmIPCInterfaceInfo(const char* service,
                       const char* path,
                       const char* interface){
        mService = QString(service);
        mPath = QString(path);
        mInterface = QString(interface);
    }

    const QString service() const { return mService; };
    const QString path() const { return mPath; };
    const QString interface() const { return mInterface; };

private:
    QString mService,
            mPath,
            mInterface;
};

class MEEGO_SYSTEM_EXPORT QmIPCInterface : protected QDBusInterface
{
    Q_OBJECT

public:
    QmIPCInterface(const QmIPCInterfaceInfo* ifInfo);
    QmIPCInterface(const char* service,
                   const char* path,
                   const char* interface);
    virtual ~QmIPCInterface();

    bool callSynchronously(const QString& method,
                           const QVariant& arg1 = QVariant(),
                           const QVariant& arg2 = QVariant());
    bool callAsynchronously(const QString& method,
                            const QVariant& arg1 = QVariant(),
                            const QVariant& arg2 = QVariant());
    QList<QVariant> get(const QString& method,
                        const QVariant& arg1 = QVariant(),
                        const QVariant& arg2 = QVariant());
    bool connect(const QString & name, QObject * receiver, const char * slot);
};

} // MeeGo namespace

#endif // QMIPCINTERFACE_H
