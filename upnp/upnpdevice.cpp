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
}

UPnPDevice::~UPnPDevice()
{
}

UPnPDevice::UPnPDevice(const UPnPDevice &other)
    : QObject(0)
    , m_proxy(other.m_proxy)
{
}

void UPnPDevice::wrapDevice(const QString &udn)
{
    GUPnPDeviceProxy *proxy = UPnPDeviceModel::lookup(udn);
    m_proxy = RefPtrG<GUPnPDeviceProxy>(proxy);
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
    QString url = QString::fromUtf8(gupnp_device_info_get_icon_url(di,
                                                                   "image/png",
                                                                   -1,
                                                                   48,
                                                                   48,
                                                                   TRUE,
                                                                   NULL,
                                                                   NULL,
                                                                   NULL,
                                                                   NULL));
    if (!url.isEmpty()) {
        return QUrl(url);
    }

    // device didn't have PNG icon, let's use anything we can get
    url = QString::fromUtf8(gupnp_device_info_get_icon_url(di,
                                                           NULL,
                                                           -1,
                                                           48,
                                                           48,
                                                           TRUE,
                                                           NULL,
                                                           NULL,
                                                           NULL,
                                                           NULL));
    if (!url.isEmpty()) {
        return QUrl(url);
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
