/*!
 * @file qmlocks.cpp
 * @brief QmLocks

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
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
#include "qmlocks.h"

#define DEPRECATION_WARNING "You are using a deprecated and non-functional interface. Instead of QmLocks, please use the DeviceLock interface."
#include <QDebug>

namespace MeeGo {


QmLocks::QmLocks(QObject *parent)
             : QObject(parent) {
    qWarning() << DEPRECATION_WARNING;
}

QmLocks::~QmLocks() {
}

QmLocks::State QmLocks::getState(QmLocks::Lock what) const {
    Q_UNUSED(what);
    qWarning() << DEPRECATION_WARNING;
    return QmLocks::Unknown;
}

QmLocks::State QmLocks::getStateAsync(QmLocks::Lock what) const {
    Q_UNUSED(what);
    qWarning() << DEPRECATION_WARNING;
    return QmLocks::Unknown;
}

bool QmLocks::setState(QmLocks::Lock what, QmLocks::State how) {
    Q_UNUSED(how);
    qWarning() << DEPRECATION_WARNING;
    return false;
}

bool QmLocks::setDeviceAutolockTime(int seconds) {
    Q_UNUSED(seconds);
    qWarning() << DEPRECATION_WARNING;
    return false;
}

int QmLocks::getDeviceAutolockTime() {
    qWarning() << DEPRECATION_WARNING;
    return -1;
}

} // namspace MeeGo
