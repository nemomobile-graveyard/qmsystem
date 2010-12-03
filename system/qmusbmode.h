/*!
 * @file qmusbmode.h
 * @brief Contains QmUSBMode

   <p>
   @copyright (C) 2009-2010 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>

   @scope Internal

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
#ifndef QMUSBMODE_H
#define QMUSBMODE_H

#include "system_global.h"
#include <QtCore/qobject.h>

QT_BEGIN_HEADER

namespace MeeGo
{

    class QmUSBModePrivate;


    /**
     * @scope Internal
     *
     * @brief Provides information and actions on USB state.
     *
     * This class is used to interact with the usb-moded daemon.
     * When the USB state is changed, the modeChanged signal is emitted.
     * Basically, when the cable is plugged, the signaled mode is Connected.
     * If there is a default mode other than Ask set, this will be the next usb mode.
     * Otherwise a ModeRequest signal is emitted, and the user should respond by calling setMode().
     * If the user fails to do this in 30 seconds, the mode will be set to ChargingOnly.
     * The user can also change the state by calling setMode() function.
     *
     * The normal operation is either to set the default mode to be OviSuite or MassStorage, which is then
     * used when the cable is plugged, or set the default mode to be Ask, listen for a ModeRequest signal, and
     * then set MassStorage or OviSuite mode. Transitions such as MassStorage -> OviSuite without re-plugging
     * the cable are not supported.
     */
    class QmUSBMode : public QObject
    {
        Q_OBJECT;

    public:

        /** All the possible modes and states. **/
        enum Mode {
            Connected = 0, /** < Signaled when the cable is plugged */
            DataInUse,     /** < Signaled when the chosen mode can cause data loss */
            Disconnected,  /** < Signaled when the cable is unplugged */
            MassStorage,   /** < Mass storage mode */
            ChargingOnly,  /** < Charging only mode */
            OviSuite,      /** < Ovi suite mode */
            ModeRequest,   /** < Signaled when the user is expected to call setMode() */
            Ask,           /** < Mode for waiting setMode() */
            Undefined      /** < Undefined */
        };

        QmUSBMode(QObject *parent = 0);
        ~QmUSBMode();

        /**
         * Gets the current mode.
         * @return the current mode.
         */
        Mode getMode();

        /**
         * Sets the mode. Note that calling setMode does not necessarily change the mode immediately.
         * To know, when the mode changed, the modeChanged signal should be listened to.
         *
         * @param mode the mode to be set. Valid modes are OviSuite, MassStorage and ChargingOnly.
         * @return true if successful
         */
        bool setMode(Mode mode);

        /**
         * Sets the default mode.
         *
         * @param mode the new default mode. Valid modes are OviSuite, MassStorage, ChargingOnly and Ask.
         * @return true if successful
         */
        bool setDefaultMode(Mode mode);

        /**
         * Gets the default mode.
         *
         * @return the default mode
         */
        Mode getDefaultMode();

    Q_SIGNALS:

        /**
         * Sent when the mode has changed
         *
         * @param mode the current mode.
         */
        void modeChanged(MeeGo::QmUSBMode::Mode mode);

        /**
         * This signal is emitted after an error occurred.
         *
         * @param errorCode a code representing the error.
         */
        void error(const QString &errorCode);

    private:
        Q_DISABLE_COPY(QmUSBMode);
        MEEGO_DECLARE_PRIVATE(QmUSBMode);

    };
}

QT_END_HEADER

#endif // QMUSBMODE_H
