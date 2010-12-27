/*!
 * @file qmcabc.cpp
 * @brief QmCABC

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Tuomo Tanskanen <ext-tuomo.1.tanskanen@nokia.com>

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
#include <QDBusReply>
#include <QDebug>

#if HAVE_MCE
    #include <mce/mode-names.h>
    #include <mce/dbus-names.h>
#endif

#include "qmcabc.h"

#include "qmipcinterface_p.h"

#if HAVE_MCE
static const QString cabc_dbus_names[] =
{
  MCE_CABC_MODE_OFF,
  MCE_CABC_MODE_UI,
  MCE_CABC_MODE_STILL_IMAGE,
  MCE_CABC_MODE_MOVING_IMAGE
};

static const int cabc_mode_num = sizeof(cabc_dbus_names)/sizeof(*cabc_dbus_names);

static int find_cabc_mode_by_dbus_name(const QString &name)
{
    for (int i=0; i < cabc_mode_num; ++i)
        if (name == cabc_dbus_names[i])
            return i;
  return -1;
}
#endif /* HAVE_MCE */

namespace MeeGo
{
    bool QmCABC::set(Mode mode)
    {
        bool success = false;
#if HAVE_MCE
        if (mode > -1 && mode < cabc_mode_num) {
            QmIPCInterface cabc(MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF);
            cabc.callAsynchronously(MCE_CABC_MODE_REQ, cabc_dbus_names[mode]);
            success = true;
        }
#endif
        return success;
    }

    QmCABC::Mode QmCABC::get() const
    {
#if HAVE_MCE
        QmIPCInterface cabc(MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF);
        QDBusReply<QString> reply = cabc.call(MCE_CABC_MODE_GET);
        if (reply.isValid()) {
            int mode = find_cabc_mode_by_dbus_name(reply.value());
            if (mode < 0) {
                qWarning() << __PRETTY_FUNCTION__ <<  "Invalid mode '" << reply.value() << "' returned by mce";
                return Off;
            } else {
               return (Mode) mode;
            }
        } else {
            return Off; // What else could be returned here?
        }
#else
        return Off;
#endif /* HAVE_MCE */
    }
}
