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

#include "upnpdevice.h"
#include "upnpmediaserver.h"
#include "upnprenderer.h"
#include "upnpdevicemodel.h"

const char UPnPDevice::CONNECTION_MANAGER_SERVICE[] = "urn:schemas-upnp-org:service:ConnectionManager";

UPnPDevice::UPnPDevice()
    : QObject(0)
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

    return QString::fromUtf8(gupnp_device_info_get_friendly_name(GUPNP_DEVICE_INFO(m_proxy)));
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

ServiceProxy UPnPDevice::getService(const char *service)
{
    if (not m_proxy.isEmpty()) {
        GUPnPServiceInfo *info = gupnp_device_info_get_service(GUPNP_DEVICE_INFO (m_proxy), service);

        return ServiceProxy::wrap(GUPNP_SERVICE_PROXY(info));
    }

    return ServiceProxy ();
}

void UPnPDevice::propagateError(GError *error)
{
    QMetaObject::invokeMethod(this, "error",
                              Qt::QueuedConnection,
                              Q_ARG(int, error->code),
                              Q_ARG(QString, QString::fromUtf8(error->message)));

    g_error_free(error);
}

void UPnPDevice::propagateError(const GError *error)
{
    QMetaObject::invokeMethod(this, "error",
                              Qt::QueuedConnection,
                              Q_ARG(int, error->code),
                              Q_ARG(QString, QString::fromUtf8(error->message)));
}
