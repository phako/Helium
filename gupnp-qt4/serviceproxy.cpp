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

#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QVariantList>

#include "refptrg.h"
#include "serviceproxy.h"
#include "serviceproxy_p.h"
#include "serviceintrospection_p.h"

/*!
 * \class ServiceProxy
 * \brief Proxy class for a remote UPnP service
 *
 * Proxy class to send commands to a remote service. connect to remote
 * event notifications and introspect the remote service.
 *
 * An ansynchronous remote call is done by call(). This prepares a
 * ServiceProxyCall object.
 *
 * Connecting to a remote event is a two-step process:
 * Fist, adding the notification with addNotify(). Second, reception of
 * events has to be enabled with setSubsribed(). With this two-step process
 * it's possible to suspend notification without unsubscribing
 *
 * The remote service's introspection information is available using
 * introspection(). This introspeciton is only available on demand and needs
 * to be requested using introspect(). Once the introspection is done,
 * ServiceProxy emits the introspectionReady() signal. After this signal
 * introspection() will return a ServiceIntrospeciton object.
 */

/*!
 * \brief Callback for gupnp_service_proxy_add_notify().
 *        Takes the variable and translates it to the notify
 * \sa ServiceProxy::addNotify(), ServiceProxy::removeNotify(), ServiceProxy::subscribed(), ServiceProxy::setSubscribed()
 * \param proxy A GUPnPServiceProxy
 * \param variable Name of the notified variable
 * \param value A GValue containing the variable's value
 * \param user_data Pointer to an instance of ServiceProxyPrivate
 */
void ServiceProxyPrivate::onNotify(GUPnPServiceProxy *proxy, const char *variable, GValue *value, gpointer user_data)
{
    Q_UNUSED(proxy);

    ServiceProxyPrivate *self = static_cast<ServiceProxyPrivate *>(user_data);

    QString var = QString::fromUtf8(variable);
    QVariant val = QVariant::fromValue(QString::fromUtf8(g_value_get_string(value)));

    QMetaObject::invokeMethod(self->q_ptr, "notify",
                              Qt::QueuedConnection,
                              Q_ARG(QString, var),
                              Q_ARG(QVariant, val));
}

/*!
 * \brief Call-back for gupnp_service_info_get_introspection_async
 * \param info Associcated GUPnPServiceInfo
 * \param introspection an instance of GUPnPServiceIntrospection
 * \param error GError, 0 on success.
 * \param user_data Pointer to a ServiceProxyPrivate object.
 */
void ServiceProxyPrivate::onIntrospection(GUPnPServiceInfo *info, GUPnPServiceIntrospection *introspection, const GError *error, gpointer user_data)
{
    ServiceProxyPrivate *self = static_cast<ServiceProxyPrivate *>(user_data);
    Q_UNUSED(info);

    self->m_introspection = new ServiceIntrospection();
    self->m_introspection->d_ptr->m_introspection = wrap(introspection);

    QMetaObject::invokeMethod(self->q_ptr, "introspectionReady", Qt::QueuedConnection);
}

/*!
 * \brief Get the service's introspection object
 * \return a ServiceIntrospection object or 0 if introspectionReady() has not
 * yet been emitted.
 * \sa introspect(),introspectionReady()
 */
ServiceIntrospection *ServiceProxy::introspection()
{
    Q_D(ServiceProxy);

    return d->m_introspection;
}

/*!
 * \brief Create a shallow ServiceProxy.
 *        This service proxy does not have a backing GUPnPServiceProxy.
 * \param parent The QObject's parent.
 */
ServiceProxy::ServiceProxy(QObject *parent)
    : QObject(parent)
    , d_ptr(new ServiceProxyPrivate(this, 0))
{
}

/*!
 * \brief Destructor.
 */
ServiceProxy::~ServiceProxy()
{
    delete d_ptr;
}

/*!
 * \def _ADD_ARG(name,value)
 *
 * \brief A macro to convert the pseudo-varargs from ServiceProxy::call() to QLists
 */
#define _ADD_ARG(name,value) \
    do { \
        if (not (name).isEmpty() && (value).isValid()) { \
            names << (name); \
            values << (value); \
        } else { \
            goto finalize; \
        } \
    } while (false);

/*!
 * \brief Prepare an UPnP call on a remote device.
 *
 * \sa ServiceProxyCall::run()
 * \param action Name of the remote action
 * \param name1 Name of the 1st argument
 * \param arg1 Value of the 1st argument
 * \param name2 Name of the 2nd argument
 * \param arg2 Value of the 2nd argument
 * \param name3 Name of the 3rd argument
 * \param arg3 Value of the 2rd argument
 * \param name4 Name of the 4th argument
 * \param arg4 Value of the 4th argument
 * \param name5 Name of the 5th argument
 * \param arg5 Value of the 5th argument
 * \param name6 Name of the 6th argument
 * \param arg6 Value of the 6th argument
 * \param name7 Name of the 7th argument
 * \param arg7 Value of the 7th argument
 * \param name8 Name of the 8th argument
 * \param arg8 Value of the 8th argument
 * \param name9 Name of the 9th argument
 * \param arg9 Value of the 9th argument
 * \param name10 Name of the 10th argument
 * \param arg10 Value of the 10th argument
 * \return A ServiceProxyCall object
 */
ServiceProxyCall *ServiceProxy::call(const QString &action,
                                     const QString &name1, const QVariant &arg1,
                                     const QString &name2, const QVariant &arg2,
                                     const QString &name3, const QVariant &arg3,
                                     const QString &name4, const QVariant &arg4,
                                     const QString &name5, const QVariant &arg5,
                                     const QString &name6, const QVariant &arg6,
                                     const QString &name7, const QVariant &arg7,
                                     const QString &name8, const QVariant &arg8,
                                     const QString &name9, const QVariant &arg9,
                                     const QString &name10, const QVariant &arg10)
{
    QStringList names;
    QVariantList values;

    _ADD_ARG(name1,arg1);
    _ADD_ARG(name2,arg2);
    _ADD_ARG(name3,arg3);
    _ADD_ARG(name4,arg4);
    _ADD_ARG(name5,arg5);
    _ADD_ARG(name6,arg6);
    _ADD_ARG(name7,arg7);
    _ADD_ARG(name8,arg8);
    _ADD_ARG(name9,arg9);
    _ADD_ARG(name10,arg10);

finalize:
    return new ServiceProxyCall(this, action, names, values);
}

/*!
 * \brief Subscribe to an evented state variable
 *
 * Value changes will be signalled via the notify() signal.
 *
 * \sa notify(), setSubscribed(), removeNotify()
 * \param variable Name of the state variable.
 */
void ServiceProxy::addNotify(const QString &variable)
{
    Q_D(ServiceProxy);

    gupnp_service_proxy_add_notify(d->m_proxy, variable.toUtf8().constData(),
                                   G_TYPE_STRING, ServiceProxyPrivate::onNotify, d);
}

/*!
 * \brief Unsubscribe from a state variable.
 *
 * Remove subscription to value changes previously registered with addNotify()
 * \param variable Name of the state variable.
 */
void ServiceProxy::removeNotify(const QString &variable)
{
    Q_D(ServiceProxy);

    gupnp_service_proxy_remove_notify(d->m_proxy, variable.toUtf8().constData(),
                                      ServiceProxyPrivate::onNotify, d);
}

/*!
 * \brief Enable or disable the subscription to events
 * \param subscribed
 */
void ServiceProxy::setSubscribed(bool subscribed)
{
    Q_D(ServiceProxy);

    gupnp_service_proxy_set_subscribed(d->m_proxy, subscribed ? TRUE : FALSE);
}

/*!
 * \brief Check the subscription state of the ServiceProxy.
 * A return value of true does not necessary mean that it emits notify() sigals.
 * The service proxy needs to be subscribed to a value with addNotify() first.
 * \sa addNotify(), removeNotify(), notify(), setSubscribed()
 * \return true, if subscribed to evets, false otherwise.
 */
bool ServiceProxy::subscribed(void) const
{
    Q_D(const ServiceProxy);

    return gupnp_service_proxy_get_subscribed(d->m_proxy) == TRUE;
}

bool ServiceProxy::isNull(void) const
{
    Q_D (const ServiceProxy);

    return d->m_proxy.isEmpty();
}

/*!
 * \brief Start asynchronous service introspection.
 *
 * Once the introspeciton is ready, the introspectionReady() signal is
 * emitted. The introspection object is available after signal emission using
 * introspection().
 *
 * \sa introspection(), introspectionReady()
 */
void ServiceProxy::introspect(void)
{
    Q_D(ServiceProxy);

    if (d->m_introspection != 0) {
        Q_EMIT introspectionReady();
    } else {
        gupnp_service_info_get_introspection_async(GUPNP_SERVICE_INFO(d->m_proxy),
                                                   ServiceProxyPrivate::onIntrospection,
                                                   d);
    }
}
