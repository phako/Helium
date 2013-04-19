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

#include "glib-utils.h"

#include "upnpdevice.h"
#include "upnpmediaserver.h"
#include "upnprenderer.h"
#include "upnpdevicemodel.h"
#include "serviceproxy_p.h"

const char UPnPDevice::CONNECTION_MANAGER_SERVICE[] = "urn:schemas-upnp-org:service:ConnectionManager";

UPnPDevice::UPnPDevice()
    : QObject(0)
    , m_pendingCalls()
    , m_proxy()
{
    connect(UPnPDeviceModel::getDefault(), SIGNAL(deviceUnavailable(QString)),
            SLOT(onDeviceUnavailable(QString)));
}

UPnPDevice::~UPnPDevice()
{
}

UPnPDevice::UPnPDevice(const UPnPDevice &other)
    : QObject(0)
    , m_proxy(other.m_proxy)
{
}

void UPnPDevice::onDeviceUnavailable(const QString &udn)
{
    if (m_proxy.isEmpty()) {
        return;
    }

    if (udn == this->udn()) {
        wrapDevice(QString());
        Q_EMIT unavailable();
    }
}

void UPnPDevice::wrapDevice(const QString &udn)
{
    if (udn.isEmpty()) {
        m_proxy = DeviceProxy();

        return;
    }

    GUPnPDeviceProxy *proxy = UPnPDeviceModel::lookup(udn);
    m_proxy = DeviceProxy(proxy);
}

QString UPnPDevice::friendlyName(void) const
{
    if (m_proxy.isEmpty()) {
        return QString();
    }

    ScopedGPointer name(gupnp_device_info_get_friendly_name(GUPNP_DEVICE_INFO(m_proxy)));

    return QString::fromUtf8(name.data());
}

QUrl UPnPDevice::getIcon (GUPnPDeviceProxy *proxy)
{
    GUPnPDeviceInfo *di = GUPNP_DEVICE_INFO(proxy);

    if (di == 0) {
        return QUrl();
    }

    // prefer PNG due to the proper transparency
    ScopedGPointer icon(gupnp_device_info_get_icon_url(di,
                                                       "image/png",
                                                       -1,
                                                       120,
                                                       120,
                                                       TRUE,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL));
    if (not icon.isNull()) {
        return QUrl(QString::fromUtf8(icon.data()));
    }

    // device didn't have PNG icon, let's use anything we can get
    icon.reset (gupnp_device_info_get_icon_url(di,
                                               NULL,
                                               -1,
                                               120,
                                               120,
                                               TRUE,
                                               NULL,
                                               NULL,
                                               NULL,
                                               NULL));
    if (not icon.isNull()) {
        return QUrl(QString::fromUtf8(icon.data()));
    }

    const char *type = gupnp_device_info_get_device_type(di);
    if (strncmp(UPnPRenderer::DEVICE_TYPE, type, strlen(UPnPRenderer::DEVICE_TYPE)) == 0) {
        return QUrl(QLatin1String("image://theme/icon-m-content-tv-show"));
    } else if (strncmp(UPnPRenderer::DEVICE_TYPE, type, strlen(UPnPMediaServer::DEVICE_TYPE)) == 0) {
        return QUrl(QLatin1String("image://theme/icon-m-common-directory"));
    }

    // generic device without any icon
    return QUrl();
}

QUrl UPnPDevice::icon() const
{
    return UPnPDevice::getIcon(m_proxy);
}

QString UPnPDevice::udn() const
{
    GUPnPDeviceInfo *info = GUPNP_DEVICE_INFO(m_proxy);

    return QString::fromUtf8(gupnp_device_info_get_udn(info));
}

QString UPnPDevice::type() const
{
    GUPnPDeviceInfo *info = GUPNP_DEVICE_INFO(m_proxy);

    return QString::fromUtf8(gupnp_device_info_get_device_type(info));
}

/*!
 * \brief Get a ServiceProxy object from the device.
 * \param service name of the service.
 * \return A ServiceProxy object representing the service or 0.
 */
ServiceProxy* UPnPDevice::getService(const char *service) const
{
    if (m_proxy.isEmpty()) {
        return 0;
    }


    auto info = gupnp_device_info_get_service(GUPNP_DEVICE_INFO(m_proxy), service);
    auto p = new ServiceProxy;
    p->d_ptr->m_proxy = wrap(GUPNP_SERVICE_PROXY(info));

    return p;
}

/*!
 * \brief Enqueue a call.
 *
 * Enqueues a call to the list of pending calls, connect the given slot to
 * ServiceProxyCall::ready() and starts the call.
 * \param call An ServiceProxyCall object
 * \param slot to call upon call completion. The default slot is
 * defaultServiceProxyCallHandler().
 */
void UPnPDevice::queueCall(ServiceProxyCall *call, const char *slot)
{
    m_pendingCalls << call;
    connect(call, SIGNAL(ready()), slot);
    call->run();
}

/*!
 * \brief Remove a finished call.
 *
 * Finalize the call with the given argument list and remove it from the list
 * of pending calls and mark the call for deletion if requested.
 *
 * \param call A ServiceProxyCall object
 * \param args List of strings of the argument names. Default is QStringList()
 * \param freeCall whether to free the call. Default is true.
 */
void UPnPDevice::unqueueCall(ServiceProxyCall *call, const QStringList &args, bool freeCall)
{
    m_pendingCalls.removeOne(call);
    call->finalize(args);
    if (freeCall) {
        call->deleteLater();
    }
}

/*!
 * \brief Default service proxy call handler.
 *
 * Finalizes the call. If the call was successful, queue the next call if
 * there is one.
 * \sa unqueueCall(), queueCall()
 */
void UPnPDevice::defaultServiceProxyCallHandler()
{
    ServiceProxyCall *call = qobject_cast<ServiceProxyCall *>(sender());

    if (call == 0) {
        return;
    }

    unqueueCall(call);

    if (call->hasError()) {
        Q_EMIT error(call->errorCode(), call->errorMessage());
    } else {
        if (call->next() != 0) {
            queueCall(call);
        }
    }
}

/*!
 * \brief Check if any remote calls are pending.
 * \return true if there are unfinished calls, false otherwise.
 */
bool UPnPDevice::callsPending(void) const
{
    return not m_pendingCalls.empty();
}
