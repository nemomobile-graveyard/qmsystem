/*!
 * @file qmdevicemode.cpp
 * @brief QmDeviceMode

   <p>
   Copyright (C) 2009-2011 Nokia Corporation

   @author Antonio Aloisio <antonio.aloisio@nokia.com>
   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Raimo Vuonnala <raimo.vuonnala@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
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
#include "qmdevicemode.h"
#include "qmdevicemode_p.h"

#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMessage>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QMetaMethod>
#endif

/*
 * PSM stuff (spec)
 *
 * Signal emitted from the com.nokia.mce.signal interface
 *
 *   Name        powersave_mode_ind
 *   Parameters  dbus_bool_t  mode TRUE (=on)/ FALSE(=off)
 *   Description Sent when the powersave mode is changed
 *
 *
 * Generic method calls provided by the com.nokia.mce.request interface
 *
 *   Name                   get_powersave_mode
 *   Parameters             -
 *   Errors / Return value  dbus_bool_t inactivity TRUE / FALSE
 *   Description            Get the current powersave mode. TRUE if the powersave mode is on,  FALSE if the powersave mode is off.
 *
 *
 *   Name                   set_powersave_mode
 *   Parameters             dbus_bool_t  mode TRUE (=on)/ FALSE(=off)
 *   Errors / Return value  dbus_bool_t  status TRUE=success / FALSE=failure
 *   Description            Set the current powersave mode. TRUE if the powersave mode is on,  FALSE if the powersave mode is off.
 *
 *
 * We also need to set the automatic PSM, GConf would be good as this is persistent setting (see qmdisplaystate.cpp)
 *
 */

namespace MeeGo
{
    QmDeviceMode::QmDeviceMode(QObject *parent) : QObject(parent) {
        MEEGO_INITIALIZE(QmDeviceMode)

        connect(priv, SIGNAL(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState)), this,
                SIGNAL(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState)));
        connect(priv, SIGNAL(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode)), this,
                SIGNAL(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode)));
    }

    QmDeviceMode::~QmDeviceMode() {
        MEEGO_PRIVATE(QmDeviceMode)

        disconnect(priv, SIGNAL(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState)), this,
                   SIGNAL(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState)));
        disconnect(priv, SIGNAL(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode)), this,
                   SIGNAL(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode)));

        MEEGO_UNINITIALIZE(QmDeviceMode);
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    void QmDeviceMode::connectNotify(const QMetaMethod &signal) {
#else
    void QmDeviceMode::connectNotify(const char *signal) {
#endif
        MEEGO_PRIVATE(QmDeviceMode)

        /* QObject::connect() needs to be thread-safe */
        QMutexLocker locker(&priv->connectMutex);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        if (signal == QMetaMethod::fromSignal(&QmDeviceMode::deviceModeChanged)) {
#else
        if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode))))) {
#endif
            if (0 == priv->connectCount[SIGNAL_DEVICE_MODE]) {
                #if HAVE_MCE
                    QDBusConnection::systemBus().connect(MCE_SERVICE,
                                                         MCE_SIGNAL_PATH,
                                                         MCE_SIGNAL_IF,
                                                         MCE_RADIO_STATES_SIG,
                                                         priv,
                                                         SLOT(deviceModeChangedSlot(const quint32)));
                #endif
            }
            priv->connectCount[SIGNAL_DEVICE_MODE]++;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        } else if (signal == QMetaMethod::fromSignal(&QmDeviceMode::devicePSMStateChanged)) {
#else
        } else if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState))))) {
#endif
            if (0 == priv->connectCount[SIGNAL_PSM_MODE]) {
                #if HAVE_MCE
                    QDBusConnection::systemBus().connect(MCE_SERVICE,
                                                         MCE_SIGNAL_PATH,
                                                         MCE_SIGNAL_IF,
                                                         MCE_PSM_STATE_SIG,
                                                         priv,
                                                         SLOT(devicePSMChangedSlot(bool)));
                #endif
            }
            priv->connectCount[SIGNAL_PSM_MODE]++;
        }
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    void QmDeviceMode::disconnectNotify(const QMetaMethod &signal) {
#else
    void QmDeviceMode::disconnectNotify(const char *signal) {
#endif
        MEEGO_PRIVATE(QmDeviceMode)

        /* QObject::disconnect() needs to be thread-safe */
        QMutexLocker locker(&priv->connectMutex);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        if (signal == QMetaMethod::fromSignal(&QmDeviceMode::deviceModeChanged)) {
#else
        if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(deviceModeChanged(MeeGo::QmDeviceMode::DeviceMode))))) {
#endif
            priv->connectCount[SIGNAL_DEVICE_MODE]--;

            if (0 == priv->connectCount[SIGNAL_DEVICE_MODE]) {
                #if HAVE_MCE
                    QDBusConnection::systemBus().disconnect(MCE_SERVICE,
                                                            MCE_SIGNAL_PATH,
                                                            MCE_SIGNAL_IF,
                                                            MCE_RADIO_STATES_SIG,
                                                            priv,
                                                            SLOT(deviceModeChangedSlot(const quint32)));
                #endif
            }
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        } else if (signal == QMetaMethod::fromSignal(&QmDeviceMode::devicePSMStateChanged)) {
#else
        } else if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState))))) {
#endif
            priv->connectCount[SIGNAL_PSM_MODE]--;

            if (0 == priv->connectCount[SIGNAL_PSM_MODE]) {
                #if HAVE_MCE
                    QDBusConnection::systemBus().disconnect(MCE_SERVICE,
                                                            MCE_SIGNAL_PATH,
                                                            MCE_SIGNAL_IF,
                                                            MCE_PSM_STATE_SIG,
                                                            priv,
                                                            SLOT(devicePSMChangedSlot(bool)));
                #endif
            }
        }
    }

    QmDeviceMode::DeviceMode QmDeviceMode::getMode() const {
        QmDeviceMode::DeviceMode deviceMode = Error;
        #if HAVE_MCE
            MEEGO_PRIVATE_CONST(QmDeviceMode)

            QDBusReply<quint32> radioStatesReply = QDBusConnection::systemBus().call(
                                                       QDBusMessage::createMethodCall(MCE_SERVICE, MCE_REQUEST_PATH,
                                                                                      MCE_REQUEST_IF, MCE_RADIO_STATES_GET));
            if (radioStatesReply.isValid()) {
                deviceMode = priv->radioStateToDeviceMode(radioStatesReply.value());
            }
        #endif
        return deviceMode;
    }

    QmDeviceMode::PSMState QmDeviceMode::getPSMState() const {
        QmDeviceMode::PSMState psmState = PSMError;
        #if HAVE_MCE
            MEEGO_PRIVATE_CONST(QmDeviceMode)

            QDBusReply<bool> psmModeReply = QDBusConnection::systemBus().call(
                                                QDBusMessage::createMethodCall(MCE_SERVICE, MCE_REQUEST_PATH,
                                                                               MCE_REQUEST_IF, MCE_PSM_STATE_GET));
            if (psmModeReply.isValid()) {
                psmState = priv->psmStateToModeEnum(psmModeReply.value());
            }
        #endif
        return psmState;
    }

    bool QmDeviceMode::setMode(QmDeviceMode::DeviceMode mode) {
        #if HAVE_MCE
            MEEGO_PRIVATE(QmDeviceMode)

            quint32 state, mask;

            switch (mode) {
            case Normal:
                state = 1;
                mask = MCE_RADIO_STATE_MASTER;
                break;
            case Flight:
                state = 0;
                mask = MCE_RADIO_STATE_MASTER;
                break;
            default:
                return false;
            }

            priv->requestIf->callAsynchronously(MCE_RADIO_STATES_CHANGE_REQ, state, mask);
            return true;
        #else
            Q_UNUSED(mode);
        #endif
        return false;
    }

    bool QmDeviceMode::setPSMState(QmDeviceMode::PSMState state) {

        // check proper state value
        bool val = false;

        if (state == PSMStateOff) {
            val = false;
        } else if (state == PSMStateOn) {
            val = true;
        } else {
            return false;
        }

        #if HAVE_MCE
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(QDBusObjectPath(FORCE_POWER_SAVING));
            argumentList << QVariant::fromValue(QDBusVariant(val));

            QDBusMessage setConfig = QDBusMessage::createMethodCall(MCE_SERVICE,
                                                                    MCE_REQUEST_PATH,
                                                                    MCE_REQUEST_IF,
                                                                    MCE_SET_CONFIG);
            setConfig.setArguments(argumentList);

            (void)QDBusConnection::systemBus().call(setConfig, QDBus::NoBlock);

            return true;
        #endif

        return false;
    }

    bool QmDeviceMode::setPSMBatteryMode(int percentages) {

        bool ret = false;
        bool enable_psm = false;

        if (percentages < 0 || percentages > 100) {
            return ret;
        }

        #if HAVE_MCE
            if (percentages > 0) {
                enable_psm = true;

                // Get the list of possible threshold values.
                QList<QVariant> getArgumentList;
                getArgumentList << QVariant::fromValue(QDBusObjectPath(THRESHOLDS));

                QDBusMessage getConfig = QDBusMessage::createMethodCall(MCE_SERVICE,
                                                                        MCE_REQUEST_PATH,
                                                                        MCE_REQUEST_IF,
                                                                        MCE_GET_CONFIG);
                getConfig.setArguments(getArgumentList);

                QDBusReply<QDBusVariant> getPossibleDisplayBlankTimeoutsReply = QDBusConnection::systemBus().call(getConfig);

                // Check if the percentage value is in the list of possible values.
                if (getPossibleDisplayBlankTimeoutsReply.isValid()) {
                    const QDBusArgument possibleValues = getPossibleDisplayBlankTimeoutsReply.value().variant().value<QDBusArgument>();
                    if (possibleValues.currentType() == QDBusArgument::ArrayType) {
                        possibleValues.beginArray();
                        int value;
                        while (!possibleValues.atEnd()) {
                            possibleValues >> value;
                            if (percentages <= value || possibleValues.atEnd()) {
                                // Set the threshold value.
                                QList<QVariant> argumentList;
                                argumentList << QVariant::fromValue(QDBusObjectPath(THRESHOLD));
                                argumentList << QVariant::fromValue(QDBusVariant(value));

                                QDBusMessage setConfig = QDBusMessage::createMethodCall(MCE_SERVICE,
                                                                                        MCE_REQUEST_PATH,
                                                                                        MCE_REQUEST_IF,
                                                                                        MCE_SET_CONFIG);
                                setConfig.setArguments(argumentList);

                                (void)QDBusConnection::systemBus().call(setConfig, QDBus::NoBlock);

                                break;
                            }
                        }
                        possibleValues.endArray();
                    }
                }
            }

            // Enable or disable psm according to percentages value
            QList<QVariant> argumentList;
            argumentList << QVariant::fromValue(QDBusObjectPath(ENABLE_POWER_SAVING));
            argumentList << QVariant::fromValue(QDBusVariant(enable_psm));

            QDBusMessage setConfig = QDBusMessage::createMethodCall(MCE_SERVICE,
                                                                    MCE_REQUEST_PATH,
                                                                    MCE_REQUEST_IF,
                                                                    MCE_SET_CONFIG);
            setConfig.setArguments(argumentList);

            (void)QDBusConnection::systemBus().call(setConfig, QDBus::NoBlock);

            ret = true;
        #endif

        return ret;
    }   

    int QmDeviceMode::getPSMBatteryMode() {

        int ret = -1;
        bool psm_enabled = false;

        #if HAVE_MCE
            QList<QVariant> argumentListEnab;
            argumentListEnab << QVariant::fromValue(QDBusObjectPath(ENABLE_POWER_SAVING));

            QDBusMessage getEnabConfig = QDBusMessage::createMethodCall(MCE_SERVICE,
                                                                        MCE_REQUEST_PATH,
                                                                        MCE_REQUEST_IF,
                                                                        MCE_GET_CONFIG);
            getEnabConfig.setArguments(argumentListEnab);

            QDBusReply<QDBusVariant> getPSMBatteryModeEnabReply = QDBusConnection::systemBus().call(getEnabConfig);

            if (getPSMBatteryModeEnabReply.isValid()) {
                psm_enabled = getPSMBatteryModeEnabReply.value().variant().toBool();
            } else {
                return ret;
            }

            if (psm_enabled == false) {
                return 0;
            }

            QList<QVariant> argumentListThres;
            argumentListThres << QVariant::fromValue(QDBusObjectPath(THRESHOLD));

            QDBusMessage getThresConfig = QDBusMessage::createMethodCall(MCE_SERVICE,
                                                                         MCE_REQUEST_PATH,
                                                                         MCE_REQUEST_IF,
                                                                         MCE_GET_CONFIG);
            getThresConfig.setArguments(argumentListThres);

            QDBusReply<QDBusVariant> getPSMBatteryModeThresReply = QDBusConnection::systemBus().call(getThresConfig);

            if (getPSMBatteryModeThresReply.isValid()) {
                ret = getPSMBatteryModeThresReply.value().variant().toInt();
            }

        #endif

        return ret;
    }

} // namespace MeeGo
