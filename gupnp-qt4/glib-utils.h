/*
This file is part of Helium.

Helium is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Helium is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Helium.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GLIBUTILS_H
#define GLIBUTILS_H

#include <glib.h>
#include <glib-object.h>

#include <QtCore/QScopedPointer>
#include <QtCore/QVariant>

template<typename C>
struct ScopedPointerDestroyHelperGFree
{
    static inline void cleanup(C *pointer)
    {
        if (pointer != 0) {
            g_free ((gpointer)pointer);
        }
    }
};
typedef QScopedPointer<char, ScopedPointerDestroyHelperGFree<char> > ScopedGPointer;

template<typename C>
struct ScopedPointerLater
{
    static inline void cleanup(C *pointer) {
        if (pointer != 0) {
            pointer->deleteLater();
        }
    }
};
typedef QScopedPointer<QObject, ScopedPointerLater<QObject> > ScopedLaterPointer;

template<typename C>
struct ScopedPointerDestroyHelperGList
{
    static inline void cleanup(C *pointer)
    {
        if (pointer != 0) {
            g_list_free(pointer);
        }
    }
};
typedef QScopedPointer<GList, ScopedPointerDestroyHelperGList<GList> > QGListScopedPointer;

template<typename C>
struct ScopedPointerDestroyHelperGListFull
{
    static inline void cleanup(C *pointer)
    {
        if (pointer != 0) {
            g_list_free_full(pointer, g_free);
        }
    }
};
typedef QScopedPointer<GList, ScopedPointerDestroyHelperGListFull<GList> > QGListFullScopedPointer;

QVariant gValueToQVariant(const GValue *other);
GValue *qVariantToGValue(const QVariant &other);

#endif // GLIBUTILS_H
