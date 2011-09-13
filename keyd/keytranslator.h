/*!
 * @file keytranslator.h
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
#ifndef KEYTRANSLATOR_H
#define KEYTRANSLATOR_H

#include <QObject>

#include <linux/types.h>
#include <linux/input.h>

class KeyTranslator : public QObject
{
    Q_OBJECT

private:
    bool longPress;
    size_t repeatCount;

    void handleKeyDown();
    void handleKeyRepeat();
    void handleKeyUp();

    static struct timeval monotime();

public:
    __u16 shortPressKey;
    __u16 longPressKey;

    KeyTranslator(QObject *parent = 0);
    ~KeyTranslator();

    void handleEvent(struct input_event &ev);

Q_SIGNALS:
    void keyTranslated(struct input_event &event);
};

#endif // KEYTRANSLATOR_H
