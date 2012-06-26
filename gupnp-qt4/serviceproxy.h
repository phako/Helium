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

#ifndef SERVICEPROXY_H
#define SERVICEPROXY_H

#include <libgupnp/gupnp.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>

#include "serviceproxycall.h"
#include "serviceintrospection.h"

class ServiceProxyPrivate;
class UPnPDevice;
class ServiceProxy : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ServiceProxy)
    Q_PROPERTY(bool subscribed READ subscribed WRITE setSubscribed)
public:
    ~ServiceProxy();

    ServiceProxyCall *call(const QString &action,
                           const QString &name1 = QString(), const QVariant &arg1 = QVariant(),
                           const QString &name2 = QString(), const QVariant &arg2 = QVariant(),
                           const QString &name3 = QString(), const QVariant &arg3 = QVariant(),
                           const QString &name4 = QString(), const QVariant &arg4 = QVariant(),
                           const QString &name5 = QString(), const QVariant &arg5 = QVariant(),
                           const QString &name6 = QString(), const QVariant &arg6 = QVariant(),
                           const QString &name7 = QString(), const QVariant &arg7 = QVariant(),
                           const QString &name8 = QString(), const QVariant &arg8 = QVariant(),
                           const QString &name9 = QString(), const QVariant &arg9 = QVariant(),
                           const QString &name10 = QString(), const QVariant &arg10 = QVariant());
    void addNotify(const QString &variable);
    void removeNotify(const QString &variable);
    void setSubscribed(bool subscribed);
    bool subscribed(void) const;
    bool isNull(void) const;

    void introspect(void);
    ServiceIntrospection *introspection(void);

Q_SIGNALS:
    void notify(const QString &variable, const QVariant &value);
    void introspectionReady(void);

private:
    explicit ServiceProxy(QObject *parent = 0);
    ServiceProxyPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(ServiceProxy)
    friend class ServiceProxyCall;
    friend class UPnPDevice;
};

#endif // SERVICEPROXY_H
