/*!
 * @file mainpage.h
 * @brief Documentation

 * @copyright Nokia Corporation
 * @license LGPL Lesser General Public License

   <p>
   @copyright (C) 2009-2011 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Timo Rongas <ext-timo.rongas.nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>

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

@mainpage LibQmSystem2 API documentation

@section introsection Introduction

QmSystem is a library that provides interfaces to different system functionalities and hardware through the Qt API. Examples of such functionalities are energy management, system state management, device mode, accessories, display backlight control, and time management.

To access the sensors (such as accelerometer, compass, magnetometer, orientation, proximity, rotation, and ambient light sensor), it is recommended that you use the Sensors API. Sensors API is included in the Qt Mobility API.

@section basicusage Basic Usage

QmSystem includes the following main classes:

    - QmActivity: provides information about device activity.
    - QmBattery: provides information on device battery status.
    - QmCABC: provides methods to set and get Content Adaptive Display Control settings.
    - QmDeviceMode: provides information and actions on device operation mode and power save mode.
    - QmDisplayState: provides information and actions on device display state.
    - QmHeartbeat: provides system heartbeat service.
    - QmKeys: provides access to hardware key states.
    - QmLocks: provides information and actions on device locks.
    - QmSystemState: provides information and actions on device state.
    - QmThermal: provides information on device thermal states.
    - QmTime: provides interface for interacting with system clock.

To get library compiler and linker parameters, use the following:<br/>
QMake: \code CONFIG+=qmsystem2 \endcode <br/>
Pkg-Config: \code pkg-config --libs --cflags qmsystem2 \endcode <br/> 

*/
