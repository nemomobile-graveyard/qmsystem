/*!
 * @file keytranslator.cpp
 * @brief KeyTranslator

   <p>
   Copyright (C) 2011 Nokia Corporation

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

#include "keytranslator.h"

#include <sys/time.h>

#include <time.h>
#include <syslog.h>

const static bool   debug                = false;
const static int    longPressTimeoutInMs = 1000;

KeyTranslator::KeyTranslator(QObject *parent) :
    QObject(parent),
    state(WAIT_FOR_KEYPRESS)
{
    longPressTimer.setSingleShot(true);
    connect(&longPressTimer, SIGNAL(timeout()), this, SLOT(longPressTimeout()));
}

KeyTranslator::~KeyTranslator()
{
}

void KeyTranslator::handleEvent(struct input_event &ev)
{
    if (debug) {
        syslog(LOG_DEBUG,
               "handleEvent, ev.time=%ld.%06ld, ev.type=%i, ev.code=%i, ev.value=%08x",
               ev.time.tv_sec,
               ev.time.tv_usec,
               ev.type,
               ev.code,
               ev.value);
    }

    if (ev.value == 1) {
        state = WAIT_FOR_LONG_PRESS;
        handleKeyDown();
    } else if (ev.value == 2) {
        /* ignore key repeats */
    } else if (ev.value == 0) {
        handleKeyUp();
        state = WAIT_FOR_KEYPRESS;
    }
}

void KeyTranslator::handleKeyDown()
{
    if (longPressTimer.isActive()) {
        /* this should never happen, as we should get KeyUp
           before KeyDown, but let's make sure */
        longPressTimer.stop();
    }

    longPressTimer.start(longPressTimeoutInMs);

    if (debug) {
        syslog(LOG_DEBUG,
               "handleKeyDown");
    }
}

void KeyTranslator::handleKeyUp()
{
    struct input_event keyDownEvent, keyUpEvent;

    memset(&keyDownEvent, 0, sizeof keyDownEvent);
    memset(&keyUpEvent, 0, sizeof keyUpEvent);

    if (longPressTimer.isActive()) {
        /* long press timer is still running,
           so this was a short press */
        longPressTimer.stop();

        /* since this was a short press, send the KeyDown
           event */
        keyDownEvent.time = monotime();
        keyDownEvent.type = EV_KEY;
        keyDownEvent.code = shortPressKey;
        keyDownEvent.value = 1;

        emit keyTranslated(keyDownEvent);
    }

    /* now send the KeyUp event */
    keyUpEvent.time = monotime();
    keyUpEvent.type = EV_KEY;
    keyUpEvent.code = (state == LONG_PRESS_DETECTED ? longPressKey : shortPressKey);
    keyUpEvent.value = 0;

    emit keyTranslated(keyUpEvent);
}

void KeyTranslator::longPressTimeout()
{
    struct input_event keyDownEvent;

    memset(&keyDownEvent, 0, sizeof keyDownEvent);

    if (state != WAIT_FOR_LONG_PRESS) {
       /* we shouldn't have an active timer if
          we are not waiting for a long press */
       return;
    }

    /* long press detected, send the KeyDown event */
    keyDownEvent.time = monotime();
    keyDownEvent.type = EV_KEY;
    keyDownEvent.code = longPressKey;
    keyDownEvent.value = 1;

    emit keyTranslated(keyDownEvent);

    state = LONG_PRESS_DETECTED;
}

struct timeval KeyTranslator::monotime()
{
    struct timeval tv;
    struct timespec ts = { 0, 0 };

    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        TIMESPEC_TO_TIMEVAL(&tv, &ts);
    }
    return tv;
}
