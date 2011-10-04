/*!
 * @file qmsysteminformation.h
 * @brief Contains QmSystemInformation.

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Matias Muhonen <ext-matias.muhonen@nokia.com>

   @scope Nokia Meego

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
#ifndef QMSYSTEMINFORMATION_H
#define QMSYSTEMINFORMATION_H

#include "system_global.h"

#include <QObject>

namespace MeeGo {

class QmSystemInformationPrivate;

/*!
 *
 * @scope Nokia Meego
 *
 * This class can be used to retrieve information from the system information service.
 *
 * The retrieve the version of the operating system, see the following example:
 *

\code
#include <qmsysteminformation.h>

MeeGo::QmSystemInformation::OperatingSystemVersion release =
    MeeGo::QmSystemInformation::operatingSystemVersion();

if (release >= MeeGo::QmSystemInformation::OSVersion_Harmattan_1_2) {
    // It is PR1.2
}

\endcode

 */

class MEEGO_SYSTEM_EXPORT QmSystemInformation : public QObject
{
    Q_OBJECT

public:
    /*!
     * Represents the version of the operating system.
     */
    enum OperatingSystemVersion {
        /**
         * Unknown version
         */
        OSVersion_Unknown          = 0,
        /**
         * Version 1.0
         */
        OSVersion_Harmattan_1_0    = 100,
        /**
         * Version 1.1
         */
        OSVersion_Harmattan_1_1    = 110,
        /**
         * Version 1.1.1
         */
        OSVersion_Harmattan_1_1_1  = 111,
        /**
         * Version 1.2
         */
        OSVersion_Harmattan_1_2    = 120
    };

    /*!
     * @brief Gets the operating system version.
     * @param firmwareVersion Optional: the firmware version string. If empty, the firmware version is retrieved from the system information service.
     * @return Returns the operating system version.
     */
    static inline QmSystemInformation::OperatingSystemVersion operatingSystemVersion(QString firmwareVersion = "")
    {
        if (firmwareVersion.isEmpty()) {
            MeeGo::QmSystemInformation systemInformation;
            firmwareVersion = systemInformation.valueForKey("/device/sw-release-ver");
        }

        if (!firmwareVersion.startsWith("DFL61_HARMATTAN_") || firmwareVersion.length() < 20) {
            return OSVersion_Unknown;
        }

        int majorVersion = firmwareVersion.at(16).digitValue();
        int minorVersion = firmwareVersion.at(17).digitValue();

        if (majorVersion >= 3) {
            return OSVersion_Harmattan_1_2;
        } else if (majorVersion == 2 && minorVersion >= 2) {
            return OSVersion_Harmattan_1_1_1;
        } else if (majorVersion == 2) {
            return OSVersion_Harmattan_1_1;
        } else if (majorVersion == 1) {
            return OSVersion_Harmattan_1_0;
        }

        return OSVersion_Unknown;
    }

    QmSystemInformation();
    virtual ~QmSystemInformation();

    /*!
     * @brief Gets the value of a key from system information.
     * @return Returns the value as a QString. Note: the value can be empty if the key contains an empty value or there was an error retrieving the value.
     */
    QString valueForKey(const QString &key) const;

    /*!
     * @brief Gets the value of a key from the kernel command line.
     * @return Returns the kernel command line value as a QString
     */
    static QString kernelCommandLineValueForKey(const QString &key);

private:
    Q_DISABLE_COPY(QmSystemInformation)
    MEEGO_DECLARE_PRIVATE(QmSystemInformation)
};

} // MeeGo namespace

#endif // QMSYSTEMINFORMATION_H
