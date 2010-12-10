/*!
 * @file qmdisplaystate_p.h
 * @brief Contains QmDisplayStatePrivate

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
#ifndef QMDISPLAYSTATE_P_H
#define QMDISPLAYSTATE_P_H

#include "qmdisplaystate.h"
#include "qmipcinterface.h"

#if HAVE_MCE
    #include "mce/dbus-names.h"
    #include "mce/mode-names.h"
#endif

#include <gconf/gconf-client.h>

#include <QDebug>

//Strings received from the D-Bus daemon
#define OFF     "off"
#define DIMMED  "dimmed"
#define ON      "on"

//Strings for GConf keys.
#define GCONF_DISPLAY_DIR "/system/osso/dsm/display"
#define MAX_BRIGHTNESS_KEY GCONF_DISPLAY_DIR "/" "max_display_brightness_levels"
#define BRIGHTNESS_KEY GCONF_DISPLAY_DIR "/" "display_brightness"
#define BLANK_TIMEOUT_KEY GCONF_DISPLAY_DIR "/" "display_blank_timeout"
#define DIM_TIMEOUT_KEY GCONF_DISPLAY_DIR "/" "display_dim_timeout"
#define BLANKING_CHARGING_KEY GCONF_DISPLAY_DIR "/" "inhibit_blank_mode"
#define POSSIBLE_DIM_LIST_KEY GCONF_DISPLAY_DIR "/" "possible_display_dim_timeouts"

namespace MeeGo
{

    class QmDisplayStatePrivate : public QObject
    {
        Q_OBJECT;
        MEEGO_DECLARE_PUBLIC(QmDisplayState)

    public:
        QmDisplayStatePrivate(){
#if HAVE_MCE
            signalIf = new QmIPCInterface(
                        MCE_SERVICE,
                        MCE_SIGNAL_PATH,
                        MCE_SIGNAL_IF);
            requestIf = new QmIPCInterface(
                        MCE_SERVICE,
                        MCE_REQUEST_PATH,
                        MCE_REQUEST_IF);
#endif
            g_type_init ();
            gc = gconf_client_get_default();

#if HAVE_MCE
            signalIf->connect(MCE_DISPLAY_SIG, this, SLOT(slotDisplayStateChanged(const QString&)));
#endif
        }

        ~QmDisplayStatePrivate(){
#if HAVE_MCE
            delete signalIf;
            delete requestIf;
#endif
            g_object_unref (gc);
        }

        QmIPCInterface *requestIf;
        QmIPCInterface *signalIf;
        GConfClient *gc;

    Q_SIGNALS:
        void displayStateChanged(MeeGo::QmDisplayState::DisplayState);

    private Q_SLOTS:

        void slotDisplayStateChanged(const QString& state){
#if HAVE_MCE
            if (state == MCE_DISPLAY_OFF_STRING)
                emit displayStateChanged(QmDisplayState::Off);
            else if (state == MCE_DISPLAY_DIM_STRING)
                emit displayStateChanged(QmDisplayState::Dimmed);
            else if (state == MCE_DISPLAY_ON_STRING)
                emit displayStateChanged(QmDisplayState::On);
#else
    Q_UNUSED(state);
#endif
        }
    };
}
#endif // QMDISPLAYSTATE_P_H
