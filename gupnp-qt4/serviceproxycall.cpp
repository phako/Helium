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
#include <gio/gio.h>

#include <QtCore/QMap>

#include "refptrg.h"
#include "glib-utils.h"
#include "serviceproxycall.h"
#include "serviceproxy.h"
#include "serviceproxy_p.h"

class ServiceProxyCallPrivate
{
public:
    ServiceProxyCallPrivate(ServiceProxyCall *parent,
                            GUPnPServiceProxy *proxy,
                            const QString     &action,
                            const QStringList &names,
                            const QVariantList &values);
    ~ServiceProxyCallPrivate();

    static void onAction(GUPnPServiceProxy       *proxy,
                         GUPnPServiceProxyAction *action,
                         gpointer                 user_data);

    void setReady(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action);

    ServiceProxyCall * const q_ptr;
    Q_DECLARE_PUBLIC(ServiceProxyCall)

    GUPnPServiceProxy * const m_proxy;
    QString m_actionName;
    GUPnPServiceProxyAction *m_action;
    bool m_ready;
    QStringList m_names;
    QVariantList m_values;
    GError *m_lastError;
    QMap<QString, QVariant> m_results;
    ServiceProxyCall *m_next;
};

ServiceProxyCallPrivate::ServiceProxyCallPrivate(ServiceProxyCall   *parent,
                                                 GUPnPServiceProxy  *proxy,
                                                 const QString      &action,
                                                 const QStringList  &names,
                                                 const QVariantList &values)
    : q_ptr(parent)
    , m_proxy(GUPNP_SERVICE_PROXY(g_object_ref(proxy)))
    , m_actionName(action)
    , m_ready(false)
    , m_names(names)
    , m_values(values)
    , m_lastError(0)
    , m_results()
    , m_next(0)
{
}

ServiceProxyCallPrivate::~ServiceProxyCallPrivate()
{
    if (m_lastError != 0) {
        g_error_free(m_lastError);
    }

    if (m_proxy != 0) {
        g_object_unref(m_proxy);
    }

    delete m_next;
}

void ServiceProxyCallPrivate::onAction(GUPnPServiceProxy       *proxy,
                                       GUPnPServiceProxyAction *action,
                                       gpointer                 user_data)
{
    ServiceProxyCallPrivate *self = static_cast<ServiceProxyCallPrivate *>(user_data);
    self->setReady(proxy, action);
}

void ServiceProxyCallPrivate::setReady(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action)
{
    Q_Q(ServiceProxyCall);

    m_ready = proxy == m_proxy && action == m_action;
    if (m_ready) {
        QMetaObject::invokeMethod(q, "ready", Qt::QueuedConnection);
    }
}

ServiceProxyCall::ServiceProxyCall(ServiceProxy *parent,
                                   const QString &action,
                                   const QStringList &params,
                                   const QVariantList &values)
    : QObject(parent)
    , d_ptr(new ServiceProxyCallPrivate(this,
                                        parent->d_ptr->m_proxy,
                                        action,
                                        params,
                                        values))
{
}

ServiceProxyCall::~ServiceProxyCall()
{
    cancel();
    delete d_ptr;
}

void ServiceProxyCall::run(void)
{
    Q_D(ServiceProxyCall);
    GList *inNames = 0, *inValues = 0;

    Q_FOREACH(QString name, d->m_names) {
        inNames = g_list_append(inNames, (gpointer) g_strdup(name.toUtf8().constData()));
    }
    QGListFullScopedPointer names(inNames);

    Q_FOREACH(QVariant value, d->m_values) {
        auto gvalue = qVariantToGValue(value);
        inValues = g_list_append(inValues, (gpointer)gvalue);
    }

    if (d->m_lastError != 0) {
        g_error_free(d->m_lastError);
        d->m_lastError = 0;
    }

    d->m_action = gupnp_service_proxy_begin_action_list(d->m_proxy,
                                                        d->m_actionName.toUtf8().constData(),
                                                        inNames,
                                                        inValues,
                                                        ServiceProxyCallPrivate::onAction,
                                                        d);

    names.reset();
    auto it = inValues;
    while (it != 0) {
        g_value_reset((GValue *)it->data);
        g_free(it->data);
        it = it->next;
    }
}

void ServiceProxyCall::cancel(void)
{
    Q_D(ServiceProxyCall);

    if (d->m_ready || d->m_action == 0) {
        // only need to cancel if action hasn't already returned
        return;
    }

    gupnp_service_proxy_cancel_action (d->m_proxy, d->m_action);
    d->m_lastError = g_error_new_literal(G_IO_ERROR, G_IO_ERROR_CANCELLED, "Action cancelled by user");
    d->m_action = 0;

    Q_EMIT ready();
}

/*!
 * \brief Check if a call was cancelled.
 * \return true if the call was previously cancelled (by cancel()), false otherwise.
 */
bool ServiceProxyCall::cancelled(void) const
{
    Q_D(const ServiceProxyCall);

    return (d != 0 && hasError() && errorCode() == G_IO_ERROR_CANCELLED);
}

#define _ADD_ARG(arg) \
    do { \
        if (not (arg).isEmpty()) { \
            outNames = g_list_append(outNames, (gpointer) g_strdup((arg).toUtf8().constData())); \
            outTypes = g_list_append(outTypes, (gpointer) G_TYPE_STRING); \
        } else { \
            goto finalize; \
        } \
    } while (false);


void ServiceProxyCall::finalize(const QStringList &params)
{
    Q_D(ServiceProxyCall);

    if (d->m_action == 0) {
        // finalize called, cancel() called or not started yet
        return;
    }

    GList *outNames = 0, *outTypes = 0, *outValues = 0;

    Q_FOREACH(const QString &name, params) {
        outNames = g_list_append(outNames, (gpointer)g_strdup(name.toUtf8().constData()));
        outTypes = g_list_append(outTypes, GSIZE_TO_POINTER(G_TYPE_STRING));
    }

    QGListFullScopedPointer names(outNames);
    QGListScopedPointer types(outTypes);

    gboolean result = gupnp_service_proxy_end_action_list(d->m_proxy,
                                                          d->m_action,
                                                          &(d->m_lastError),
                                                          outNames, outTypes, &outValues);
    d->m_action = 0;
    if (not result) {
        return;
    }

    GList *it = outValues, *it2 = outNames;

    while (it != 0) {
        d->m_results.insert(QString::fromUtf8((const char *)it2->data),
                            QVariant::fromValue(QString::fromUtf8(g_value_get_string((GValue *)it->data))));
        it = it->next;
        it2 = it2->next;
    }

    names.reset();
    types.reset();
}

bool ServiceProxyCall::hasError(void) const
{
    Q_D(const ServiceProxyCall);

    return d->m_lastError != 0;
}

int ServiceProxyCall::errorCode(void) const
{
    Q_D(const ServiceProxyCall);

    if (not hasError()) {
        return 0;
    }

    return d->m_lastError->code;
}

QString ServiceProxyCall::errorMessage(void) const
{
    Q_D(const ServiceProxyCall);

    if (not hasError()) {
        return QString();
    }

    return QString::fromUtf8(d->m_lastError->message);
}

QVariant ServiceProxyCall::get(const QString &key) const
{
    Q_D(const ServiceProxyCall);

    return d->m_results[key];
}

QVariant ServiceProxyCall::arg(const QString &arg) const
{
    Q_D(const ServiceProxyCall);

    int pos = d->m_names.indexOf(arg);
    if (pos >= d->m_values.size()) {
        return QVariant();
    }

    return d->m_values.at(pos);
}

void ServiceProxyCall::setArg(const QString &arg, const QVariant &value)
{
    Q_D(ServiceProxyCall);

    int pos = d->m_names.indexOf(arg);
    if (pos >= d->m_values.size()) {
        return;
    }

    d->m_values.replace(pos,value);
}

/*!
 * \brief Get the next prepared ServiceProxyCall.
 *
 * \sa netNext()
 * \return 0 if there is no next call, a pointer to the next call otherwise.
 */
ServiceProxyCall *ServiceProxyCall::next() const
{
    Q_D(const ServiceProxyCall);

    return d->m_next;
}

/*!
 * \brief Set the next call in a call chain.
 *
 * Sometimes it is useful to make several calls in a row. The next call pointer
 * can be used to save a prepared next call. Note that the chain is not
 * executed automatically, you need to run the call yourself.
 *
 * \param next the next proxy call in line.
 */
void ServiceProxyCall::setNext(ServiceProxyCall *next)
{
    Q_D(ServiceProxyCall);

    d->m_next = next;
}
