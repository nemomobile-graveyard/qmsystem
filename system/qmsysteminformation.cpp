/*!
 * @file qmsysteminformation.cpp
 * @brief QmSystemInformation

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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
#include "qmsysteminformation.h"
#include "qmsysteminformation_p.h"

namespace MeeGo
{

QmSystemInformationPrivate::QmSystemInformationPrivate()
{
    #if HAVE_SYSINFO
    systemConfig = 0;
    if (sysinfo_init(&systemConfig) != 0) {
        /* Failed to initialize the system configuration object */
        if (systemConfig) {
            sysinfo_finish(systemConfig), systemConfig = 0;
        }
    }
    #endif
}

QmSystemInformationPrivate::~QmSystemInformationPrivate()
{
    #if HAVE_SYSINFO
    if (systemConfig) {
        sysinfo_finish(systemConfig), systemConfig = 0;
    }
    #endif
}

QString QmSystemInformationPrivate::valueForKey(const QString &key) const
{
    QString value("");
    #if HAVE_SYSINFO
    uint8_t *data = 0;
    unsigned long size = 0;

    if (!systemConfig) {
        goto EXIT;
    }

    if (sysinfo_get_value(systemConfig, key.toStdString().c_str(), &data, &size) != 0) {
        /* Failed to read the key from the system configuration */
        goto EXIT;
    }

    for (unsigned long k=0; k < size; k++) {
        /* Values can contain non-ascii data -> escape those */
        int c = data[k];
        if (c < 32 || c > 126)
            continue;
        value += (char)c;
    }
    #endif
    EXIT:
    return value;
}

QmSystemInformation::QmSystemInformation()
{
     MEEGO_INITIALIZE(QmSystemInformation);
}

QmSystemInformation::~QmSystemInformation()
{
    MEEGO_UNINITIALIZE(QmSystemInformation);
}

QString QmSystemInformation::valueForKey(const QString &key) const
{
    MEEGO_PRIVATE_CONST(QmSystemInformation)
    return priv->valueForKey(key);
}

} //MeeGo namespace
