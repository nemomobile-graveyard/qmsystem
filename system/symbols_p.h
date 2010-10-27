/*!
 * @file symbols_p.h
 * @brief Deprecated and not used.

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Ilya Dogolazky <ilya.dogolazky@nokia.com>
   @author Timo Olkkonen <ext-timo.p.olkkonen@nokia.com>
   @author Timo Rongas <ext-timo.rongas.nokia.com>

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
#ifndef SYMBOLS_P_H
#define SYMBOLS_P_H
#include <QtCore/qglobal.h>

namespace MeeGo {

# define DEFINEFUNC(libname, ret, func, args, argcall, funcret)        \
    typedef ret (* _q_PTR_##func) args;                                \
    static inline ret q_##func args                                    \
    {                                                                  \
        static _q_PTR_##func ptr;                                      \
        if (!ptr)                                                      \
            ptr = (_q_PTR_##func) symbols_resolve_me(libname, #func); \
        funcret ptr argcall;                                           \
    }

/**
 * Resolved a symbol in given shared library.
 * @param libname Name of the library.
 * @param name Name of the symbol.
 * @return Void pointer to the resolved symbol.
 * <b>This function should be called only through #DEFINEFUNC -macro.</b>
 */
void *symbols_resolve_me(const char *libname, const char *name);

/**
 * Loads a shared library.
 * @param libname Name of the shared library.
 * @return True on success, false on failure.
 */
bool symbols_load_library(const char *libname);

/**
 * Unloads all shared libraries loaded by #symbols_load_library().
 */
void symbols_unload_libraries();

} // namespace MeeGo

#endif // TIMESYMBOLS_P_H
