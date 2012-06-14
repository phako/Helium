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

#ifndef SERVICEPROXY_P_H
#define SERVICEPROXY_P_H

#include <libgupnp/gupnp.h>

#include <QtCore/QObject>

#include "refptrg.h"

struct ServiceProxyPrivate
{
    ServiceProxyPrivate(ServiceProxy      *parent,
                        GUPnPServiceProxy *proxy)
        : q_ptr(parent)
        , m_proxy(proxy)
        , m_introspection(0) {}
    ~ServiceProxyPrivate() { /* do nothing */  }

    static void onNotify(GUPnPServiceProxy *proxy, const char *variable, GValue *value, gpointer user_data);
    static void onIntrospection(GUPnPServiceInfo *info, GUPnPServiceIntrospection *introspection, const GError *error, gpointer user_data);

    ServiceProxy * const q_ptr;
    Q_DECLARE_PUBLIC(ServiceProxy)
    GServiceProxy m_proxy;
    ServiceIntrospection * m_introspection;
};

#endif // SERVICEPROXY_P_H
