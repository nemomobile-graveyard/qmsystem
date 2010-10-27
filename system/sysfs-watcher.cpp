/*!
 * @file sysfs-watcher.cpp
 * @brief Deprecated and not used.

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

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
#include <cassert>
#include <cstdlib>

#include <QDebug>
#include <QObject>
#include <QSocketNotifier>

#include "sysfs-watcher.h"

SysfsWatcher::SysfsWatcher(const QString &p, QObject *parent) : QObject(parent), path(p), file(0), watcher(0)
{
  file = new QFile(p) ;
  bool res = file->open(QIODevice::ReadOnly) ;
  if (!res)
  {
    delete file ;
    file = NULL ;
    qWarning() << __PRETTY_FUNCTION__ << "Can't open" << p ;
    return ;
  }
  read_content() ;
  watcher = new QSocketNotifier (file->handle(), QSocketNotifier::Read) ;
  connect(watcher, SIGNAL(activated(int)), this, SLOT(ready(int))) ;
}

SysfsWatcher::~SysfsWatcher()
{
  delete watcher;
  delete file;
}

void SysfsWatcher::read_content()
{
  // should we care about multithreading here ?
  // workaround by Stefano, for Qt 4.6: doing this 2 times!
  QByteArray data ;
  for (int i=0; i<2; ++i)
  {
    file->seek(0) ;
    data = file->readAll() ;
  }
  if(data.length()==0)
    qWarning() << __PRETTY_FUNCTION__ << "empty read in" << path ;
#if 0
  else
    qDebug() << __PRETTY_FUNCTION__ << data.length() << "bytes read from" << path ;
  QString cmd_line = "cat " + path ;
  system(cmd_line.toStdString().c_str()) ;
#endif
  content = data ;
}

bool SysfsWatcher::is(const char *state)
{
  const char *p = content ;
  return strncmp(p, state, content.length()) == 0 && state[content.length()]=='\0' ;
}

void SysfsWatcher::ready(int fd)
{
  // should we care about multithreading here ?
  assert(fd==file->handle()) ;
  read_content() ;
  emit content_changed() ; // even if it's the same as before
}
