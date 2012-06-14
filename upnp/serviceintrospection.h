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

#ifndef SERVICEINTROSPECTION_H
#define SERVICEINTROSPECTION_H

#include <QtCore/QObject>
#include <QtCore/QVariant>

struct ServiceProxyStateVariable {
    // Fill with the rest once we need it
    QVariant maximum() { return m_maximum; }

    QVariant m_maximum;
};

class ServiceProxyPrivate;
class ServiceIntrospectionPrivate;
class ServiceIntrospection : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ServiceIntrospection)
public:
    explicit ServiceIntrospection(QObject *parent = 0);
    ~ServiceIntrospection();

    bool hasAction(const QString &action) const;
    ServiceProxyStateVariable variable(const QString &varName) const;
private:
    ServiceIntrospectionPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(ServiceIntrospection)
    friend class ServiceProxyPrivate;
};

#endif // SERVICEINTROSPECTION_H
