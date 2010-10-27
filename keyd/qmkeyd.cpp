/*!
 * @file qmkeyd.cpp
 * @brief QmKeyd

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Yang Yang <ext-yang.25.yang@nokia.com>
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

#include "qmkeyd.h"

#include <cstdio>
#include <fcntl.h>
#include <syslog.h>
#include <sys/inotify.h>
#include <QFile>

#include <sys/types.h>
#include <sys/socket.h>

#include <string.h>



#define GPIO_KEYS "/dev/input/gpio-keys"
#define KEYPAD "/dev/input/keypad"
#define ECI "/dev/input/eci"

#ifndef KEY_CAMERA_FOCUS
#define KEY_CAMERA_FOCUS 0x210
#endif
#ifndef SW_KEYPAD_SLIDE
#define SW_KEYPAD_SLIDE 0x0a
#endif

#define EVENT_SIZE (sizeof( struct inotify_event))
#define BUF_LEN (1024* (EVENT_SIZE+16))

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

static int  debugmode = 0;

QmKeyd::QmKeyd(int argc, char**argv) : QCoreApplication(argc, argv)
{
    gpioFile = -1;
    keypadFile = -1;
    eciFile = -1;
    btFile = -1;
    users = 0;
    openlog("qmkeyd", LOG_NDELAY|LOG_PID, LOG_DAEMON);

    if (argc > 1 && !strcmp(argv[1], "-d"))
        debugmode = 1;

    server = new QLocalServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    cleanSocket();

    if (!server->listen(SERVER_NAME)) {
        syslog(LOG_CRIT, "Failed to listen incoming connections on %s, exit\n", SERVER_NAME);
        cleanSocket();
        QCoreApplication::exit(1);
    }

    if (chmod(SERVER_NAME, S_IRWXU|S_IRWXG|S_IRWXO) != 0) {
        syslog(LOG_CRIT, "Could not set permissions %s, exit\n", SERVER_NAME);
        cleanSocket();
        QCoreApplication::exit(1);
    }


    /*dynamically detect bluetooth device*/
    int fd, wd;
    fd = inotify_init();
    wd = inotify_add_watch( fd, "/dev/input", IN_CREATE | IN_DELETE);
    if (fd != -1) {
        inputNotifier = new QSocketNotifier(fd, QSocketNotifier::Read);
        connect(inputNotifier, SIGNAL(activated(int)), this, SLOT(detectBT(int)));
    } else {
        syslog(LOG_WARNING, "Could not open /dev/input\n");
        inputNotifier = NULL;
    }
}

QmKeyd::~QmKeyd()
{
    server->close();
    delete server;
    closelog();
    delete inputNotifier;
    inputNotifier = NULL;

    closeHandles();

    if (btNotifier != NULL) {
        close(btFile);
        btFile = -1;
        delete btNotifier;
        btNotifier = NULL;
    }
}

/*Check if the newly created device is BT headset, if not return false*/
bool QmKeyd::isHeadset(int fd)
{
    int yalv;
    int mediaDevice = 0;
    int mediaKeys = 0;
    unsigned long bit[EV_MAX][NBITS(KEY_MAX)];
    memset(bit, 0, sizeof(bit));
    ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);

    for (yalv = 0; yalv < EV_MAX; yalv++) {
        if (test_bit(yalv, bit[0])) {
            switch (yalv) {
            case EV_SYN :
                mediaDevice++;
                break;
            case EV_KEY :
                mediaDevice++;
                ioctl(fd, EVIOCGBIT(yalv, KEY_MAX), bit[yalv]);
                for (int j = 0; j < KEY_MAX; j++) {
                    if (test_bit(j, bit[yalv])) {
                        switch (j) {
                        case KEY_PAUSECD: mediaKeys++; break;
                        case KEY_PLAYCD: mediaKeys++; break;
                        case KEY_STOPCD: mediaKeys++; break;
                        case KEY_NEXTSONG: mediaKeys++; break;
                        case KEY_FASTFORWARD: mediaKeys++; break;
                        case KEY_PREVIOUSSONG: mediaKeys++; break;
                        case KEY_REWIND: mediaKeys++; break;break;
                        }
                    }
                }
                break;
            case EV_REL :
                mediaDevice++;
                break;
            case EV_REP :
                mediaDevice++;
                break;
            }
        }
    }
    if (mediaDevice == 4 && mediaKeys > 0)
        return true;

    return false;
}
void QmKeyd::cleanSocket()
{
    QFile serverSocket(SERVER_NAME);
    if (serverSocket.exists()) {
        /* If a socket exists but we fail to delete it, it can be a sign of a potential
         * race condition. Therefore, exit the process as it is a critical failure.
         */
        if (!serverSocket.remove()) {
            syslog(LOG_CRIT, "Could not clean the existing socket %s, exit\n", SERVER_NAME);
            QCoreApplication::exit(1);
        }
    }
}

void QmKeyd::detectBT(int fd)
{
    int len = 0;
    struct inotify_event *event;
    char buffer[BUF_LEN];

    len = read( fd, buffer, BUF_LEN );
    event =  (struct inotify_event *) &buffer[0];
    if (event->mask == IN_CREATE) {
        strcpy(btFileDir, "/dev/input/");
        strcat(btFileDir, event->name);
        btFile = open(btFileDir, O_RDONLY | O_NONBLOCK);        
        if (btFile != -1) {
            if (isHeadset(btFile)) {
                btNotifier = new QSocketNotifier(btFile, QSocketNotifier::Read);
                connect(btNotifier, SIGNAL(activated(int)), this, SLOT(readyRead(int)));
            } else {
                close(btFile);
                btFile = -1;
            }
        } else {
            syslog(LOG_WARNING, "Could not open %s\n", btFileDir);
            btNotifier = NULL;
        }
    } else if (event->mask == IN_DELETE && btFile != -1) {
        close(btFile);
        btFile = -1;
        delete btNotifier;
        btNotifier = NULL;
    }
}

void QmKeyd::newConnection()
{
    while (server->hasPendingConnections()) {
        QLocalSocket *socket = server->nextPendingConnection();
        connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
        connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
        connections.push_back(socket);
        users++;

        if (debugmode) {

            struct ucred cr;
            socklen_t    cl = sizeof(cr);
            pid_t        pid = 0;
            int          fd = socket->socketDescriptor(); 

            if (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &cr, &cl) == 0) {
                pid = cr.pid;
                // printf("Peer's pid=%d, uid=%d, gid=%d\n", cr.pid, cr.uid, cr.gid);
            }
          
            syslog(LOG_DEBUG, "New client with PID %u, socket %p, clients now %d\n", (unsigned int)pid, socket, users);
        }

        if (users >= 1)
            openHandles();
    }
}

void QmKeyd::disconnected()
{
    QLocalSocket *socket = qobject_cast<QLocalSocket*>(sender());
    for (QVector<QLocalSocket*>::iterator it = connections.begin(); it != connections.end(); it++) {
        if (*it == socket) {

            users--;

            if (debugmode) {
                syslog(LOG_DEBUG, "Client with socket %p disappeared, clients now %d\n", socket, users);
            }

            connections.erase(it);

            if (!users)
                closeHandles();
            break;
        }
    }
    socket->deleteLater();
}

void QmKeyd::socketReadyRead() {
    QLocalSocket *socket = qobject_cast<QLocalSocket*>(sender());
    if (!socket) {
        syslog(LOG_WARNING, "An invalid socket to be read\n");
        return;
    }
    struct input_event ev;
    int ret = socket->read((char*)&ev, sizeof(ev));
    if (ret == sizeof(ev)) {
        QList<int> fdList = readFrom(ev);
        int fd;        
        /*The EVIOCGKEY ioctl is used to determine the global key and button state for a device.
         *EVIOCGKEY sets a bit in the bit array for each key or button that is pressed.
         */
        foreach (fd, fdList) {
            if (fd != -1 && ioctl(fd, EVIOCGKEY(sizeof(keys)), keys) != -1) {
                if (((keys[ev.code/8]) & (1 << (ev.code % 8))) != 0) {
                    ev.value = 1;
                    break;
                } else {
                    ev.value = 0;
                }
            } else {
                ev.value = 0;
            }
        }
        if (socket->write((char*)&ev, sizeof(ev)) != sizeof(ev)) {
            int          fd = socket->socketDescriptor(); 
            syslog(LOG_WARNING, "Could not write to a socket %d\n", fd);
        }
    }
}

void QmKeyd::readyRead(int fd)
{
    int ret = 0;
    do {
        struct input_event ev;
        ret = read(fd, &ev, sizeof(ev));
        if (ret == sizeof(ev) && !readFrom(ev).isEmpty()) {
            QLocalSocket *socket;
            foreach (socket, connections) {
                socket->write((char*)&ev, sizeof(ev));
            }
        }
    } while (ret > 0);
}

QList<int> QmKeyd::readFrom(struct input_event &ev)
{
    QList<int> list;
    if (ev.type == EV_KEY) {
        switch (ev.code) {
        case KEY_RIGHTCTRL:
            list.push_back(keypadFile);
            break;
        case KEY_CAMERA:
        case KEY_CAMERA_FOCUS:
            list.push_back(gpioFile);
            break;
        case KEY_VOLUMEUP:
        case KEY_VOLUMEDOWN:            
            list.push_back(keypadFile);
            list.push_back(eciFile);
            list.push_back(btFile);
            break;
        case KEY_UP:
        case KEY_LEFT:
        case KEY_RIGHT:
        case KEY_END:
        case KEY_DOWN:
        case KEY_MUTE:
        case KEY_STOP:
        case KEY_FORWARD:
        case KEY_PLAYPAUSE:
        case KEY_PHONE:
            list.push_back(eciFile);
            break;
        case KEY_PAUSECD:
        case KEY_PLAYCD:
        case KEY_STOPCD:
        case KEY_NEXTSONG:
        case KEY_FASTFORWARD:
        case KEY_PREVIOUSSONG:        
            list.push_back(btFile);
            break;
        case KEY_REWIND:
            list.push_back(btFile);
            list.push_back(eciFile);
            break;
        }
    } else if (ev.type == EV_SW) {
        switch (ev.code) {
        case SW_KEYPAD_SLIDE:
            list.push_back(gpioFile);
            break;
        }
    }
    return list;
}

void QmKeyd::openHandles()
{
    
    if (gpioFile == -1) {
        gpioFile = open(GPIO_KEYS, O_RDONLY | O_NONBLOCK);
        if (gpioFile != -1) {
            gpioNotifier = new QSocketNotifier(gpioFile, QSocketNotifier::Read);
            connect(gpioNotifier, SIGNAL(activated(int)), this, SLOT(readyRead(int)));
        } else {
            syslog(LOG_WARNING, "Could not open %s\n", GPIO_KEYS);
            gpioNotifier = NULL;
        }
    }

    if (keypadFile == -1) {
        keypadFile = open(KEYPAD, O_RDONLY | O_NONBLOCK);
        if (keypadFile != -1) {
            keypadNotifier = new QSocketNotifier(keypadFile, QSocketNotifier::Read);
            connect(keypadNotifier, SIGNAL(activated(int)), this, SLOT(readyRead(int)));
        } else {
            syslog(LOG_WARNING, "Could not open %s\n", KEYPAD);
            keypadNotifier = NULL;
        }
    }
    if (eciFile == -1) {
        eciFile = open(ECI, O_RDONLY | O_NONBLOCK);
        if (eciFile != -1) {
            eciNotifier = new QSocketNotifier(eciFile, QSocketNotifier::Read);
            connect(eciNotifier, SIGNAL(activated(int)), this, SLOT(readyRead(int)));
        } else {
            syslog(LOG_WARNING, "Could not open %s\n", ECI);
            eciNotifier = NULL;
        }
    }
}

void QmKeyd::closeHandles()
{
    if (gpioFile != -1) {
        close(gpioFile);
        gpioFile = -1;
        delete gpioNotifier;
        gpioNotifier = NULL;
    }

    if (keypadFile != -1) {
        close(keypadFile);
        keypadFile = -1;
        delete keypadNotifier;
        keypadNotifier = NULL;
    }

    if (eciFile != -1) {
        close(eciFile);
        eciFile = -1;
        delete eciNotifier;
        eciNotifier = NULL;
    }
}
