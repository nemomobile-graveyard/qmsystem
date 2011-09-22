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

const static bool   debug          = false;
/* after this many key repeats, the key press is interpreted as a long press */
const static size_t keyRepeatLimit = 2;

KeyTranslator::KeyTranslator(QObject *parent) :
    QObject(parent),
    longPress(false),
    repeatCount(0)
{
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
        handleKeyDown();
    } else if (ev.value == 2) {
        handleKeyRepeat();
    } else if (ev.value == 0) {
        handleKeyUp();
    }
}

void KeyTranslator::handleKeyDown()
{
    longPress = false, repeatCount = 0;

    if (debug) {
        syslog(LOG_DEBUG,
               "handleKeyDown");
    }
}

void KeyTranslator::handleKeyRepeat()
{
    ++repeatCount;

    if (debug) {
        syslog(LOG_DEBUG,
               "handleKeyRepeat, key repeated %i times",
               repeatCount);
    }
}

void KeyTranslator::handleKeyUp()
{
    struct input_event keyDownEvent, keyUpEvent;

    memset(&keyDownEvent, 0, sizeof keyDownEvent);
    memset(&keyUpEvent, 0, sizeof keyUpEvent);

    longPress = (repeatCount > keyRepeatLimit), repeatCount = 0;

    keyDownEvent.time = monotime();
    keyDownEvent.type = EV_KEY;
    keyDownEvent.code = (longPress ? longPressKey : shortPressKey);
    keyDownEvent.value = 1;

    keyUpEvent.time = monotime();
    keyUpEvent.type = EV_KEY;
    keyUpEvent.code = (longPress ? longPressKey : shortPressKey);
    keyUpEvent.value = 0;

    if (debug) {
        syslog(LOG_DEBUG,
               "handleKeyUp, emit %s", (longPress ? "long press" : "short press"));
    }

    emit keyTranslated(keyDownEvent);
    emit keyTranslated(keyUpEvent);
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
