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

#include <libgupnp/gupnp.h>

#include "refptrg.h"
#include "glib-utils.h"
#include "serviceintrospection.h"
#include "serviceintrospection_p.h"

ServiceIntrospectionPrivate::ServiceIntrospectionPrivate(ServiceIntrospection *parent,
                                                         GUPnPServiceIntrospection *introspection)
    : m_introspection(GServiceIntrospection::wrap(introspection))
    , q_ptr (parent)
{
}

ServiceIntrospection::ServiceIntrospection(QObject *parent)
    : QObject(parent)
    , d_ptr(new ServiceIntrospectionPrivate(this, 0))
{
}

ServiceIntrospection::~ServiceIntrospection()
{
    delete d_ptr;
}

/*!
 * \brief Check the availability of an action
 *
 * Note that even if this returns true, that only means that the action is
 * mentioned in the service description file. To make sure, the result code of
 * the action needs to be checked for 602 or 401 which means "Not implemented".
 *
 * \param action Name of the action
 * \return true
 */
bool ServiceIntrospection::hasAction(const QString &action) const
{
    Q_D(const ServiceIntrospection);

    return gupnp_service_introspection_get_action (d->m_introspection, action.toUtf8().constData()) != 0;
}

/*!
 * \brief Get the state variable with the name varName
 * \param varName Name of the state variable
 * \return An instance of ServiceProxyStateVariable containing information about the state variable
 */
ServiceProxyStateVariable ServiceIntrospection::variable(const QString &varName) const
{
    Q_D(const ServiceIntrospection);

    ServiceProxyStateVariable var;

    auto stateVar = gupnp_service_introspection_get_state_variable(d->m_introspection, varName.toUtf8().constData());
    if (stateVar != 0) {
        var.m_maximum = gValueToQVariant(&(stateVar->maximum));
        auto it = stateVar->allowed_values;
        while (it != 0) {
            var.m_allowedValues << QString::fromUtf8((const char *)it->data);
            it = it->next;
        }
    }

    return var;
}
