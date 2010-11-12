/*!
 * @file qmled.cpp
 * @brief QmLed

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
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
#include "qmled.h"

#include "qmipcinterface.h"
#include <mce/dbus-names.h>
#include <mce/mode-names.h>

#include <QDebug>


namespace MeeGo {

class QmLEDPrivate
{
    MEEGO_DECLARE_PUBLIC(QmLED)

public:
    QmLEDPrivate(){
        requestIf = new QmIPCInterface(
                    MCE_SERVICE,
                    MCE_REQUEST_PATH,
                    MCE_REQUEST_IF);
    }

    ~QmLEDPrivate(){
        delete requestIf;
    }

    QmIPCInterface *requestIf;
};

QmLED::QmLED(QObject *parent)
              : QObject(parent){
    MEEGO_INITIALIZE(QmLED)
}

QmLED::~QmLED(){
    MEEGO_UNINITIALIZE(QmLED);
}

bool QmLED::activate(const QString &pattern){
    MEEGO_PRIVATE(QmLED);
    return priv->requestIf->callSynchronously(MCE_ACTIVATE_LED_PATTERN, pattern);
}

bool QmLED::deactivate(const QString &pattern){
    MEEGO_PRIVATE(QmLED);
    return priv->requestIf->callSynchronously(MCE_DEACTIVATE_LED_PATTERN, pattern);
}

bool QmLED::enable(void){
    MEEGO_PRIVATE(QmLED);
    return priv->requestIf->callSynchronously(MCE_ENABLE_LED);
}

bool QmLED::disable(void){
    MEEGO_PRIVATE(QmLED);
    return priv->requestIf->callSynchronously(MCE_DISABLE_LED);
}

} //namespace MeeGo
