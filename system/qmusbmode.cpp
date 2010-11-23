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

    QmUSBMode::QmUSBMode(QObject *parent) : QObject(parent)
    {
        MEEGO_INITIALIZE(QmUSBMode);
        connect(priv, SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode)), this, SIGNAL(modeChanged(MeeGo::QmUSBMode::Mode)));
        connect(priv, SIGNAL(error(const QString&)), this, SIGNAL(error(const QString&)));
    }

    QmUSBMode::~QmUSBMode()
    {
        MEEGO_UNINITIALIZE(QmUSBMode);
    }

    QmUSBMode::Mode QmUSBMode::getMode()
    {
        MEEGO_PRIVATE(QmUSBMode);
        return priv->getMode();
    }

    bool QmUSBMode::setMode(QmUSBMode::Mode mode)
    {
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

    bool QmUSBMode::setDefaultMode(QmUSBMode::Mode mode)
    {
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

    QmUSBMode::Mode QmUSBMode::getDefaultMode()
    {
        MEEGO_PRIVATE(QmUSBMode);
        return priv->getDefaultMode();
    }
}
