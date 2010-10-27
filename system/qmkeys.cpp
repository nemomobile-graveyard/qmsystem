/*!
 * @file qmkeys.cpp
 * @brief QmKeys

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Sagar Shinde <ext-sagar.shinde@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.rongas@nokia.com>
   @author Yang Yang <ext-yang.25.yang@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

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
#include "qmkeys.h"
#include "qmlocks.h"
#include "qmkeys_p.h"

namespace MeeGo
{
    QmKeysPrivate::QmKeysPrivate(QObject *parent) : QObject(parent) {
        socket = new QLocalSocket(this);
        socket->connectToServer(SERVER_NAME);
        if (!socket->waitForConnected()) {
            qWarning() << "Could not connect to " << SERVER_NAME;
        }
        connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
        cameraFocusDown = false;
    }
    QmKeysPrivate::~QmKeysPrivate() {
        socket->disconnect();
        delete socket;
    }

    QmKeys::Key QmKeysPrivate::codeToKey(__u16 code) {
        switch (code) {
        case KEY_CAMERA:
            return QmKeys::Camera;
        case KEY_VOLUMEUP:
            return QmKeys::VolumeUp;
        case KEY_VOLUMEDOWN:
            return QmKeys::VolumeDown;
        case KEY_UP:
            return QmKeys::UpKey;
        case KEY_DOWN:
            return QmKeys::DownKey;
        case KEY_LEFT:
            return QmKeys::LeftKey;
        case KEY_RIGHT:
            return QmKeys::RightKey;
        case KEY_END:
            return QmKeys::End;
        case KEY_MUTE:
            return QmKeys::Mute;
        case KEY_STOPCD:
        case KEY_STOP:
            return QmKeys::Stop;
        case KEY_FASTFORWARD:
        case KEY_FORWARD:
            return QmKeys::Forward;
        case KEY_NEXTSONG:
            return QmKeys::NextSong;
        case KEY_PLAYPAUSE:
            return QmKeys::PlayPause;
        case KEY_PLAYCD:
            return QmKeys::Play;
        case KEY_REWIND:
            return QmKeys::Rewind;
        case KEY_PREVIOUSSONG:
            return QmKeys::PreviousSong;
        case KEY_PHONE:
            return QmKeys::Phone;
        case KEY_PAUSECD:
            return QmKeys::Pause;
        case KEY_RIGHTCTRL:
            return QmKeys::RightCtrl;
        }
    }

    struct input_event QmKeysPrivate::keyToEvent(QmKeys::Key key) {
        struct input_event ev;
        switch (key) {
        case QmKeys::KeyboardSlider:
            ev.type = EV_SW;
            ev.code = SW_KEYPAD_SLIDE;
            break;
        case QmKeys::VolumeUp:
            ev.type = EV_KEY;
            ev.code = KEY_VOLUMEUP;
            break;
        case QmKeys::VolumeDown:
            ev.type = EV_KEY;
            ev.code = KEY_VOLUMEDOWN;
            break;
        /*case QmKeys::Phone:
        case QmKeys::PlayPause:
        case QmKeys::Play:
        case QmKeys::Stop:
        case QmKeys::Forward:
        case QmKeys::Rewind:
        default:
            break;*/
        }
        return ev;
    }
    QmKeys::State QmKeysPrivate::getKeyState(QmKeys::Key key) {
        if (keyMap.find(key) != keyMap.end()) {
            return keyMap.value(key);
        }
        if (key == QmKeys::Camera) {
            struct input_event query;
            query.type = EV_KEY;
            query.code = KEY_CAMERA_FOCUS;
            int focus = getKeyValue(query);
            query.code = KEY_CAMERA;
            int camera = getKeyValue(query);
            if (focus == 0 && camera == 0) {
                return QmKeys::KeyUp;
            } else if (focus == 1 && camera == 0) {
                return QmKeys::KeyHalfDown;
            } else if (focus == 1 && camera == 1) {
                return QmKeys::KeyDown;
            } else if (focus == 0 && camera == 1){
                return QmKeys::KeyDown;
            }
        } else {
            struct input_event query = keyToEvent(key);
            int value = getKeyValue(query);
            if (value == 0) {
                return QmKeys::KeyUp;
            } else if (value == 1) {
                return QmKeys::KeyDown;
            } else {
                return QmKeys::KeyInvalid;
            }
        }
    }

    int QmKeysPrivate::getKeyValue(const struct input_event &query) {

        // Try to connect to qmkeyd.
        QLocalSocket socket;
        socket.connectToServer(SERVER_NAME);
        if (!socket.waitForConnected(1000)) {
            return -1;
        }

        // Query for the given key
        if (socket.write((char*)&query, sizeof(query)) != sizeof(query)) {
            return -1;
        }
        if (!socket.waitForReadyRead(1000)) {
            return -1;
        }
        struct input_event response;
        int ret = 0;

        // A loop because we might receive other events as well.
        do {
            ret = socket.read((char*)&response, sizeof(response));
            if (ret == sizeof(response)) {
                if (response.type == query.type && response.code == query.code) {
                    break;
                }
            }
        } while (ret == sizeof(response));
        socket.disconnect();

        return response.value;
    }

    /* The logic in camera keys is as follows:
     * We have two events: KEY_CAMERA and KEY_CAMERA_FOCUS.
     * KeyUp: KEY_CAMERA == 0 && KEY_CAMERA_FOCUS == 0
     * KeyHalfDown: KEY_CAMERA == 0 && KEY_CAMERA_FOCUS == 0
     * KeyDown: KEY_CAMERA == 1 && KEY_CAMERA_FOCUS 1
     *
     * So we have few invalid states. If we receive these, we print a warning.
     * If KEY_CAMERA_FOCUS == 0 and we receive KEY_CAMERA == 1, goto KeyDown.
     * If KEY_CAMERA == 1 and we receive KEY_CAMERA_FOCUS == 0, goto KeyUp
     * If KEY_CAMERA == 1 || KEY_CAMERA_FOCUS == 1 and we receive KEY_CAMERA_FOCUS == 1, do nothing.
     */
    void QmKeysPrivate::readyRead() {

        int ret=0;
        do {
            struct input_event ev;
            ret = socket->read((char*)&ev, sizeof(ev));
            if (ret == sizeof(ev)) {
                if (ev.type == EV_KEY) {
                    switch (ev.code) {
                    case KEY_PAUSECD:
                    case KEY_UP:
                    case KEY_LEFT:
                    case KEY_RIGHT:
                    case KEY_END:
                    case KEY_DOWN:
                    case KEY_MUTE:
                    case KEY_STOP:
                    case KEY_FORWARD:
                    case KEY_PLAYPAUSE:
                    case KEY_REWIND:
                    case KEY_PREVIOUSSONG:
                    case KEY_PHONE:
                    case KEY_PLAYCD:
                    case KEY_NEXTSONG:
                    case KEY_STOPCD:
                    case KEY_FASTFORWARD:
                    case KEY_RIGHTCTRL:
                        {
                            QmKeys::Key key = codeToKey(ev.code);
                            QmKeys::State state;
                            if (ev.value == 0) {
                                state = QmKeys::KeyUp;
                            } else {
                                state = QmKeys::KeyDown;
                            }
                            keyMap[key] = state;
                            emit keyEvent(key, state);
                        }
                        break;
                    case KEY_CAMERA:
                        if (ev.value == 0) {
                            if (cameraFocusDown) {
                                keyMap[QmKeys::Camera] = QmKeys::KeyHalfDown;
                                emit cameraLauncherMoved(QmKeys::Down);
                            } else {
                                keyMap[QmKeys::Camera] = QmKeys::KeyUp;
                                emit cameraLauncherMoved(QmKeys::Up);
                            }
                        } else {
                            keyMap[QmKeys::Camera] = QmKeys::KeyDown;
                            emit cameraLauncherMoved(QmKeys::Through);
                            if (!cameraFocusDown) {
                                qWarning() << "Received a Camera down event without being half down.";
                            }
                        }
                        emit keyEvent(QmKeys::Camera, keyMap.value(QmKeys::Camera));
                        break;
                    case KEY_CAMERA_FOCUS:
                        if (ev.value == 0) {
                            cameraFocusDown = false;
                            if (!keyMap.contains(QmKeys::Camera) || keyMap.value(QmKeys::Camera) != QmKeys::KeyHalfDown) {
                                qWarning() << "Received a KEY_CAMERA_FOCUS up event without being in HalfDown state.";
                            }
                            keyMap[QmKeys::Camera] = QmKeys::KeyUp;
                            emit cameraLauncherMoved(QmKeys::Up);
                        } else {
                            cameraFocusDown = true;
                            if (!keyMap.contains(QmKeys::Camera) || keyMap.value(QmKeys::Camera) == QmKeys::KeyUp) {
                                keyMap[QmKeys::Camera] = QmKeys::KeyHalfDown;
                                emit cameraLauncherMoved(QmKeys::Down);
                            } else {
                                qWarning() << "Received a KEY_CAMERA_FOCUS down event in state " << keyMap.value(QmKeys::Camera);
                            }
                        }
                        emit keyEvent(QmKeys::Camera, keyMap.value(QmKeys::Camera));
                        break;
                    case KEY_VOLUMEUP:
                        if (ev.value == 0) {
                            keyMap[QmKeys::VolumeUp] = QmKeys::KeyUp;
                            emit volumeUpMoved(false);
                        } else  if (ev.value == 1 ) {
                            keyMap[QmKeys::VolumeUp] = QmKeys::KeyDown;
                            emit volumeUpMoved(true);
                        }
                        emit keyEvent(QmKeys::VolumeUp, keyMap.value(QmKeys::VolumeUp));
                        break;
                    case KEY_VOLUMEDOWN:
                        if (ev.value == 0) {
                            keyMap[QmKeys::VolumeDown] = QmKeys::KeyUp;
                            emit volumeDownMoved(false);
                        } else if (ev.value == 1) {
                            keyMap[QmKeys::VolumeDown] = QmKeys::KeyDown;
                            emit volumeDownMoved(true);
                        }
                        emit keyEvent(QmKeys::VolumeDown, keyMap.value(QmKeys::VolumeDown));
                        break;
                    }
                } else if (ev.type == EV_SW) {
                    switch (ev.code) {
                        case SW_KEYPAD_SLIDE:
                            if (ev.value == 0) {
                                keyMap[QmKeys::KeyboardSlider] = QmKeys::KeyUp;
                                emit keyboardSliderMoved(QmKeys::KeyboardSliderOut);
                            } else {
                                keyMap[QmKeys::KeyboardSlider] = QmKeys::KeyDown;
                                emit keyboardSliderMoved(QmKeys::KeyboardSliderIn);
                            }
                            emit keyEvent(QmKeys::KeyboardSlider, keyMap.value(QmKeys::KeyboardSlider));
                        break;
                    }
                }
            }
        } while (ret > 0);
    }

    QmKeys::QmKeys(QObject *parent) : QObject(parent) {
        priv = new QmKeysPrivate();
        connect(priv, SIGNAL(keyEvent(MeeGo::QmKeys::Key, MeeGo::QmKeys::State)), this, SIGNAL(keyEvent(MeeGo::QmKeys::Key, MeeGo::QmKeys::State)));
        connect(priv, SIGNAL(volumeDownMoved(bool)), this, SIGNAL(volumeDownMoved(bool)));
        connect(priv, SIGNAL(volumeUpMoved(bool)), this, SIGNAL(volumeUpMoved(bool)));
        connect(priv, SIGNAL(cameraLauncherMoved(QmKeys::CameraKeyPosition)), this, SIGNAL(cameraLauncherMoved(QmKeys::CameraKeyPosition)));
        connect(priv, SIGNAL(lensCoverMoved(QmKeys::LensCoverPosition)), this, SIGNAL(lensCoverMoved(QmKeys::LensCoverPosition)));
        connect(priv, SIGNAL(keyboardSliderMoved(QmKeys::KeyboardSliderPosition)), this, SIGNAL(keyboardSliderMoved(QmKeys::KeyboardSliderPosition)));
    }

    QmKeys::~QmKeys() {
        delete priv;
    }

    QmKeys::State QmKeys::getKeyState(Key key) {
        return priv->getKeyState(key);
    }

    QmKeys::KeyboardSliderPosition QmKeys::getSliderPosition() {
        if (getKeyState(KeyboardSlider) == KeyDown) {
            return KeyboardSliderIn;
        } else {
            return KeyboardSliderOut;
        }
    }

    QmKeys::LensCoverPosition QmKeys::getLensCoverPosition() {
        return Unknown;
    }

    QmKeys::CameraKeyPosition QmKeys::getCameraLauncherPosition() {
        State state = getKeyState(Camera);
        switch (state) {
        case KeyUp:
            return Up;
        case KeyHalfDown:
            return Down;
        case KeyDown:
            return Through;
        case KeyInvalid:
        default:
            return Invalid;
        }
    }

    bool QmKeys::isVolumeUpDown() {
        if (getKeyState(VolumeUp) == KeyDown) {
            return true;
        } else {
            return false;
        }
    }

    bool QmKeys::isVolumeDownDown() {
        if (getKeyState(VolumeDown) == KeyDown) {
            return true;
        } else {
            return false;
        }
    }

} // namespace MeeGo
#if 0
#include "qmkeys.h"
#include "qmipcinterface.h"

#include <QFile>
#include <QSocketNotifier>
#include <QDBusInterface>

#include <QDebug>

#include <cstdio>
#include <fcntl.h>
#include <linux/input.h>

#include <mce/dbus-names.h>

#ifndef KEY_CAMERA_FOCUS
#define KEY_CAMERA_FOCUS 0x210
#endif
#ifndef SW_KEYPAD_SLIDE
#define SW_KEYPAD_SLIDE 0x0a
#endif

#define GPIO_KEYS "/dev/input/gpio-keys"
//#define KEYPAD "/dev/input/keypad"

namespace MeeGo
{
    class QmKeysPrivate
    {
        MEEGO_DECLARE_PUBLIC(QmKeys);
    public:
        QmKeysPrivate() {
            gpioFile = open(GPIO_KEYS, O_RDONLY | O_NONBLOCK);
            //keypadFile = open(KEYPAD, O_RDONLY | O_NONBLOCK);
            gpioNotifier = new QSocketNotifier(gpioFile, QSocketNotifier::Read);
            //keypadNotifier = new QSocketNotifier(keypadFile, QSocketNotifier::Read);

            cameraPos = QmKeys::Invalid;

            sigIf = new QmIPCInterface(MCE_SERVICE, MCE_SIGNAL_PATH, MCE_SIGNAL_IF);
        }

        ~QmKeysPrivate() {
            delete gpioNotifier;
            close(gpioFile);
            //delete keypadNotifier;
            //close(keypadFile);
        }

        bool isKeyDown(int code, int fd) {
            memset(keys, 0, sizeof(keys));
            if (ioctl(fd, EVIOCGKEY(sizeof(keys)), keys) == -1) {
                return false;
            }
            if (((keys[code/8]) & (1 << (code % 8))) == 0) {
                return false;
            } else {
                return true;
            }
        }

        bool isSwitched(int code) {
            memset(switches, 0, sizeof(switches));
            if (ioctl(gpioFile, EVIOCGSW(sizeof(switches)), switches) == -1) {
                return false;
            }
            if (((switches[code/8]) & (1 << (code % 8))) == 0) {
                return false;
            } else {
                return true;
            }
        }

        uint8_t keys[KEY_MAX/8 + 1];
        uint8_t switches[SW_MAX/8 + 1];

        int gpioFile;
        //int keypadFile;
        QSocketNotifier *gpioNotifier;
        //QSocketNotifier *keypadNotifier;
        QmKeys::CameraKeyPosition cameraPos;
        QmIPCInterface *sigIf;
        QMap<int, int> keyCacheMap;
    };

    QmKeys::QmKeys(QObject *parent) : QObject(parent) {
        MEEGO_INITIALIZE(QmKeys);

        priv->cameraPos = getCameraLauncherPosition();
        connect(priv->gpioNotifier, SIGNAL(activated(int)), this, SLOT(readable(int)));
        //connect(priv->keypadNotifier, SIGNAL(activated(int)), this, SLOT(readable(int)));
        priv->sigIf->connect(MCE_INPUT_EVENT_SIG, this, SLOT(signal(unsigned, unsigned, int)));
    }

    QmKeys::~QmKeys() {
        MEEGO_UNINITIALIZE(QmKeys);
    }

    QmKeys::KeyboardSliderPosition QmKeys::getSliderPosition() {
        MEEGO_PRIVATE(QmKeys);

        if (priv->isSwitched(SW_KEYPAD_SLIDE)) {
            return KeyboardSliderIn;
        } else {
            return KeyboardSliderOut;
        }
    }

    QmKeys::LensCoverPosition QmKeys::getLensCoverPosition() {

        return Unknown;
    }

    QmKeys::CameraKeyPosition QmKeys::getCameraLauncherPosition() {
        MEEGO_PRIVATE(QmKeys);

        bool focus = priv->isKeyDown(KEY_CAMERA_FOCUS, priv->gpioFile);
        bool camera = priv->isKeyDown(KEY_CAMERA, priv->gpioFile);
        if (focus && !camera) {
            return Down;
        } else if (focus && camera) {
            return Through;
        } else if (!focus && !camera) {
            return Up;
        } else {
            return Invalid;
        }
    }

    bool QmKeys::isVolumeUpDown() {
        MEEGO_PRIVATE(QmKeys);

        int value = priv->keyCacheMap.value(KEY_VOLUMEUP, 0);
        if (value == 0) {
            return false;
        } else {
            return true;
        }
        //return priv->isKeyDown(KEY_VOLUMEUP, priv->keypadFile);
    }

    bool QmKeys::isVolumeDownDown() {
        MEEGO_PRIVATE(QmKeys);

        int value = priv->keyCacheMap.value(KEY_VOLUMEDOWN, 0);
        if (value == 0) {
            return false;
        } else {
            return true;
        }
        //return priv->isKeyDown(KEY_VOLUMEDOWN, priv->keypadFile);
    }

    void QmKeys::signal(unsigned type, unsigned code, int value) {
        MEEGO_PRIVATE(QmKeys);

        if (type == EV_KEY) {
            priv->keyCacheMap[code] = value;
            switch (code) {
            case KEY_VOLUMEUP:
                if (value == 0) {
                    emit volumeUpMoved(false);
                } else  if (value == 1 ) {
                    emit volumeUpMoved(true);
                }
                break;
            case KEY_VOLUMEDOWN:
                if (value == 0) {
                    emit volumeDownMoved(false);
                } else if (value == 1) {
                    emit volumeDownMoved(true);
                }
                break;
            }
        }
    }

    void QmKeys::readable(int fd) {

        MEEGO_PRIVATE(QmKeys);

        int ret=0;
        do {
            struct input_event ev;
            ret = read(fd, &ev, sizeof(ev));
            if (ret == sizeof(ev)) {
                if (ev.type == EV_KEY) {
                    switch (ev.code) {
                    case KEY_CAMERA:
                        if (ev.value == 0) {
                            if (priv->cameraPos == QmKeys::Through) {
                                priv->cameraPos = QmKeys::Down;
                            } else {
                                qWarning() << "Received a KEY_CAMERA up event in state " << priv->cameraPos;
                                priv->cameraPos = QmKeys::Invalid;
                            }
                        } else {
                            if (priv->cameraPos == QmKeys::Down) {
                                priv->cameraPos = QmKeys::Through;
                            } else {
                                qWarning() << "Received a KEY_CAMERA down event in state " << priv->cameraPos;
                                priv->cameraPos = QmKeys::Invalid;
                            }
                        }
                        emit cameraLauncherMoved(priv->cameraPos);
                        break;
                    case KEY_CAMERA_FOCUS:
                        if (ev.value == 0) {
                            if (priv->cameraPos == QmKeys::Down) {
                                priv->cameraPos = QmKeys::Up;
                            } else {
                                qWarning() << "Received a KEY_CAMERA_FOCUS up event in state " << priv->cameraPos;
                                priv->cameraPos = QmKeys::Invalid;
                            }
                        } else {
                            if (priv->cameraPos == QmKeys::Up) {
                                priv->cameraPos = QmKeys::Down;
                            } else {
                                qWarning() << "Received a KEY_CAMERA_FOCUS down event in state " << priv->cameraPos;
                                priv->cameraPos = QmKeys::Invalid;
                            }
                        }
                        emit cameraLauncherMoved(priv->cameraPos);
                        break;
                    }
                } else if (ev.type == EV_SW) {
                    switch (ev.code) {
                        case SW_KEYPAD_SLIDE:
                            if (ev.value == 0) {
                                emit keyboardSliderMoved(QmKeys::KeyboardSliderOut);
                            } else {
                                emit keyboardSliderMoved(QmKeys::KeyboardSliderIn);
                            }
                        break;
                    }
                }
            }
        } while (ret > 0);
    }
}
#endif

#if 0

#include <cassert>
#include <vector>
using namespace std ;

#include <QDebug>
#include <QVector>

#include <mce/dbus-names.h>
#include <mce/mode-names.h>

#include "qmkeys.h"

#include "qmipcinterface.h"
#include "msystemdbus.h"

#include "sysfs-watcher.h"

namespace MeeGo
{

// XXX Do we need those definitions? I'd like to move them to the array below one day
#define KEYBOARD_SLIDER_PATH "/sys/devices/platform/gpio-switch/slide/state"
#define CAMERA_COVER_PATH "/sys/devices/platform/gpio-switch/cam_shutter/state"
#define CAMERA_FOCUS_PATH "/sys/devices/platform/gpio-switch/cam_focus/state"
#define CAMERA_LAUNCH_PATH "/sys/devices/platform/gpio-switch/cam_launch/state"

// Files to watch
static struct file_to_watch_t
{
  const char *path ;
  const char *slot ;
} sysfs_files[] =
{
  { KEYBOARD_SLIDER_PATH, SLOT(slotSliderMoved())},
  { CAMERA_COVER_PATH, SLOT(slotLensCoverMoved())},
  { CAMERA_FOCUS_PATH,  SLOT(cameraLauncherActivated())},
  { CAMERA_LAUNCH_PATH, SLOT(cameraLauncherActivated())}
} ;

// Those constatnts should be in sync with the static structure array above
const unsigned int keyboard_slider_position_i = 0 ;
const unsigned int camera_lens_state_i = 1 ;
const unsigned int camera_focus_state_i = 2 ;
const unsigned int camera_launcher_state_i = 3 ;
const unsigned int sysfs_files_number = 4 ;

class QmKeysPrivate
{
  MEEGO_DECLARE_PUBLIC(QmKeys)

public:
  // XXX
  // I don't see why should we have _two_ constructors/destructors
  // -- one for the "public" and one for "private" things.
  // (okay, I don't even see why should we use that private classes at all)
#if 0
  // I moved it to "normal" constructor/destructor
  QmKeysPrivate()
  {
    mce_sig_if = new QmIPCInterface(MCE_SERVICE, MCE_SIGNAL_PATH, MCE_SIGNAL_IF) ;
  }
  ~QmKeysPrivate()
  {
    delete mce_sig_if ;
  }
#endif

  QmIPCInterface *mce_sig_if ;
  vector<SysfsWatcher*> watcher ;
};

QmKeys::QmKeys(QObject *parent) : QObject(parent)
{
    MEEGO_INITIALIZE(QmKeys);

    //Register the datatypes for  usage on DBus
    qRegisterMetaType<MeeGo::QmKeys::CameraKeyPosition>("MeeGo::QmKeys::CameraKeyPosition");
    qRegisterMetaType<MeeGo::QmKeys::LensCoverPosition>("MeeGo::QmKeys::LensCoverPosition");
    qRegisterMetaType<MeeGo::QmKeys::KeyboardSliderPosition>("MeeGo::QmKeys::KeyboardSliderPosition");
    qRegisterMetaType<MeeGo::QmKeys::HowPressed>("MeeGo::QmKeys::HowPressed");
    qRegisterMetaType<MeeGo::QmKeys::Keys>("MeeGo::QmKeys::Keys");

    priv->mce_sig_if = new QmIPCInterface(MCE_SERVICE, MCE_SIGNAL_PATH, MCE_SIGNAL_IF) ;

    // XXX Do we have "Home Key" at all?
    priv->mce_sig_if->connect(MCE_HOME_KEY_SIG, this, SLOT(slotHomekeyPress()));
    priv->mce_sig_if->connect(MCE_HOME_KEY_LONG_SIG, this, SLOT(slotHomekeyLongPress()));

    //priv->mce_sig_if->connect(MCE_HARDWARE_KEY_SIG, this, SLOT(hw_key_handler(const QString &,int)));

    compilation_assert(sysfs_files_number==sizeof(sysfs_files)/sizeof(file_to_watch_t)) ;

    for(unsigned i=0; i<sysfs_files_number; ++i)
    {
      SysfsWatcher *w = new SysfsWatcher(sysfs_files[i].path) ;
      if(w->is_valid())
        connect(w, SIGNAL(content_changed()), this, sysfs_files[i].slot) ;
      priv->watcher.push_back(w) ;
    }
#if 0
    keyboardStateFile.setFileName(KB_SLIDE_STATE_PATH);
    if (keyboardStateFile.exists() && keyboardStateFile.open(QIODevice::ReadOnly)) {
        keyboardNotifier = new QSocketNotifier(keyboardStateFile.handle(), QSocketNotifier::Read);
        connect(keyboardNotifier, SIGNAL(activated(int)), this, SLOT(slotSliderMoved(int)));
        keyboardNotifier->setEnabled(true);
    } else {
        keyboardNotifier = NULL;
        qWarning() << "Unable to open" << KB_SLIDE_STATE_PATH << "for monitoring";
    }

    lensCoverStateFile.setFileName(CAM_LENS_STATE_PATH);
    if (lensCoverStateFile.exists() && lensCoverStateFile.open(QIODevice::ReadOnly)) {
        lensCoverNotifier = new QSocketNotifier(lensCoverStateFile.handle(), QSocketNotifier::Read);
        connect(lensCoverNotifier, SIGNAL(activated(int)), this, SLOT(slotLensCoverMoved(int)));
        lensCoverNotifier->setEnabled(true);
    } else {
        lensCoverNotifier = NULL;
        qWarning() << "Unable to open" << CAM_LENS_STATE_PATH << "for monitoring";
    }
#endif
}

QmKeys::~QmKeys()
{
    MEEGO_PRIVATE(QmKeys);

    assert(sysfs_files_number==priv->watcher.size()) ;
    for(unsigned i=0; i<priv->watcher.size(); ++i)
      delete priv->watcher[i] ;

    delete priv->mce_sig_if ;

    MEEGO_UNINITIALIZE(QmKeys);
#if 0
    if (keyboardNotifier) {
        delete keyboardNotifier;
        keyboardStateFile.close();
    }
    if (lensCoverNotifier) {
        delete lensCoverNotifier;
        lensCoverStateFile.close();
    }
#endif
}

void QmKeys::slotHomekeyPress()
{
  emit keyPressed(QmKeys::HomeKey, QmKeys::ShortPress);
}

void QmKeys::slotHomekeyLongPress()
{
  emit keyPressed(QmKeys::HomeKey, QmKeys::LongPress);
}

void QmKeys::slotLensCoverMoved()
{
    emit lensCoverMoved(getLensCoverPosition());
}

void QmKeys::slotSliderMoved()
{
    emit keyboardSliderMoved(getSliderPosition());
}

void QmKeys::cameraLauncherActivated()
{
  emit cameraLauncherMoved(getCameraLauncherPosition()) ;
}

QmKeys::CameraKeyPosition QmKeys::getCameraLauncherPosition()
{
  MEEGO_PRIVATE(QmKeys) ;
  SysfsWatcher *focus = priv->watcher[camera_focus_state_i] ;
  SysfsWatcher *launch = priv->watcher[camera_launcher_state_i] ;
  if(!focus->is_valid())
    qWarning() << __PRETTY_FUNCTION__ << "focus watcher isn't valid" ;
  if(!launch->is_valid())
    qWarning() << __PRETTY_FUNCTION__ << "launch watcher isn't valid" ;
  if(!focus->is_valid() || !launch->is_valid())
    return Up ;
  int value = 0 ;
  if(focus->is("active\n"))
    value |= Focus_Mask ;
  if(launch->is("active\n"))
    value |= Launch_Mask ;
  if(value==Invalid)
    qWarning() << __PRETTY_FUNCTION__ << "the camera key is in invalid position" ;
  qDebug() << "focus" << focus->is("active\n") << focus->is("inactive\n") ;
  qDebug() << "launch" << launch->is("active\n") << launch->is("inactive\n") ;
  qDebug() << "value" << value ;
  return (CameraKeyPosition) value ;
}

QmKeys::KeyboardSliderPosition QmKeys::getSliderPosition()
{
  MEEGO_PRIVATE(QmKeys) ;
  SysfsWatcher *w = priv->watcher[keyboard_slider_position_i] ;
  // TODO: change the enum names from covered/uncodered to open/closed
  return w->is_valid() && w->is("open\n") ? QmKeys::KeyboardSliderOut : QmKeys::KeyboardSliderIn ;
#if 0
    if (fileSaysOpen(keyboardStateFile)) {
        return QmKeys::KeyboardSliderOut;
    }
    return QmKeys::KeyboardSliderIn;
#endif
}

QmKeys::LensCoverPosition QmKeys::getLensCoverPosition()
{
  MEEGO_PRIVATE(QmKeys) ;
  SysfsWatcher *w = priv->watcher[camera_lens_state_i] ;
  // TODO: rename as above to open/close
  return w->is_valid() && w->is("open\n") ? QmKeys::LensUnCovered : QmKeys::LensCovered ;
#if 0
    if (fileSaysOpen(lensCoverStateFile)) {
        return QmKeys::LensUnCovered;
    }
    return QmKeys::LensCovered;
#endif
}

#if 0
bool QmKeys::fileSaysOpen(QFile& file)
{
    if (!file.isReadable()) {
        qWarning() << "Invalid filehandle";
        return false;
    }

    char buff[8];
    if (file.readLine(buff, 8) < 0) {
        qWarning() << "Error reading file" << file.fileName();
        return false;
    }

    if (strncmp(buff, "open", 4)) {
        return false;
    } else {
        return true;
    }
}
#endif

void QmKeys::hw_key_handler(const QString &status, int code)
{
    /*
  if(status==MCE_KEY_STATE_UP)
    emit keyUp(code) ;
  else if(status==MCE_KEY_STATE_DOWN)
    emit keyDown(code) ;
  else if(status==MCE_KEY_STATE_REPEATING)
    emit keyAutoRepeated(code) ;
  else
    qDebug() << __PRETTY_FUNCTION__ << "invalid status=" << status << "(code=" << code << ")";
    */
}

} // namespace MeeGo
#endif
