/**
 * @file qmcabc.h
 * @brief Contains QmCABC which provides methods to set and get Content Adaptive Display Control settings.

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Timo Rongas <ext-timo.rongas.nokia.com>

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

#ifndef __QMCABC_H__
#define __QMCABC_H__

#include <QObject>
#include "system_global.h"

QT_BEGIN_HEADER

namespace MeeGo
{

/**
 *
 * @scope Nokia Meego
 *
 * @brief Provides methods to set and get Content Adaptive Display Control settings.
 */

class MEEGO_SYSTEM_EXPORT QmCABC : public QObject
{
    Q_OBJECT
    Q_ENUMS(Mode)

public:
  enum Mode
  {
    Off=0,          /**< CABC is not activated */
    Ui=1,           /**< Best quality. */   
    StillImage=2,   /**< Medium quality, suitable for images */
    MovingImage=3   /**< Suitable for video */
  } ;

  QmCABC(QObject *parent = 0) : QObject(parent) {} ;

  /**
   * Get current CABC mode.
   * @return Mode CABC mode.
   */
  Mode get() const ;


  /**
   * Set current CABC mode.
   *
   * @param mode CABC  mode.
   * @return bool TRUE if success, FALSE if failed.
   */
  bool set(Mode mode) ;
};

} // namespace MeeGo

QT_END_HEADER

#endif
