/*!
 * @file sysfs-watcher.h
 * @brief Contains SysfsWatcher

   <p>
   @copyright (C) 2009-2010 Nokia Corporation
   @license LGPL Lesser General Public License

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>

   @scope Private

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
#include <QObject>
#include <QFile>
#include <QString>
#include <QSocketNotifier>

class SysfsWatcher : public QObject
{
  Q_OBJECT;
  QString path;
  QByteArray content;
  QFile *file;
  QSocketNotifier *watcher;
private:
  void read_content();
private Q_SLOTS:
  void ready(int fd);
public:
  SysfsWatcher(const QString &p, QObject *parent=NULL);
  virtual ~SysfsWatcher();
  bool is_valid() { return file!=NULL; }
  bool is(const char *);
Q_SIGNALS:
  void content_changed();
};
