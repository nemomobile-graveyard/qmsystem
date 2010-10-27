/*!
 * @file symbols.cpp
 * @brief Deprecated and not used.

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.rongas.nokia.com>

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
#include "symbols_p.h"
#include <QtCore/qlibrary.h>
#include <QtCore/qhash.h>
#include <QDebug>


namespace MeeGo {

//static QLibrary *time_libtime = 0;

QHash<QString, QLibrary *> libraries;

bool symbols_load_library(const char *libname)
{
    QLibrary *lib;
    lib = libraries.value(libname, NULL);

    if (lib) {
        return lib && lib->isLoaded();
    }

    lib = new QLibrary(QLatin1String(libname), 0);
    libraries.insert(libname, lib);

    if (lib->load())
        return true;

    lib->unload();
    delete lib;
    lib = 0;
    return false;
}

void symbols_unload_libraries()
{
    foreach(QLibrary *lib, libraries) {
        lib->unload();
    }
    libraries.clear();
}

void *symbols_resolve_me(const char *libname, const char *name)
{
    void *ptr = 0;
    if (!symbols_load_library(libname))
        qFatal("Cannot find library '%s' in your system to resolve symbol '%s'.", libname, name);

    ptr = (libraries.value(libname))->resolve(name);
    if (!ptr)
        qFatal("Cannot resolve '%s' in your libtime.", name);

    return ptr;
}
/*
bool time_loadLibTime()
{
    static volatile bool triedToLoadLibrary = false;

    QLibrary *&lib = time_libtime;
    if (triedToLoadLibrary)
        return lib && lib->isLoaded();

    lib = new QLibrary(QLatin1String("time"), 0);
    triedToLoadLibrary = true;

    if (lib->load())
       return true;

    lib->unload();
    delete lib;
    lib = 0;
    return false;
}

void time_unloadLibTime()
{
    time_libtime->unload();
}

void *time_resolve_me(const char *name)
{
    void *ptr = 0;
    if (!time_loadLibTime())
        qFatal("Cannot find libtime in your system to resolve symbol '%s'.", name);

    ptr = time_libtime->resolve(name);
    if (!ptr)
        qFatal("Cannot resolve '%s' in your libtime.", name);

    return ptr;
}
*/
} // namespace MeeGo
