/*
This file is part of MediaController.

MediaController is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

MediaController is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with MediaController.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "upnpmediaserver.h"
#include "browsemodel.h"

#include <QDebug>
#include <QtDeclarative>

#include "browsemodelstack.h"

const char UPnPMediaServer::DEVICE_TYPE[] = "urn:schemas-upnp-org:device:MediaServer:";
const char UPnPMediaServer::CONTENT_DIRECTORY_SERVICE[] = "urn:schemas-upnp-org:service:ContentDirectory";

UPnPMediaServer::UPnPMediaServer()
    : UPnPDevice()
{
}

void UPnPMediaServer::wrapDevice(const QString &udn)
{
    UPnPDevice::wrapDevice(udn);
    if (not m_proxy.isEmpty()) {
        GUPnPServiceInfo *info = gupnp_device_info_get_service(GUPNP_DEVICE_INFO (m_proxy),
                                                               UPnPMediaServer::CONTENT_DIRECTORY_SERVICE);
        m_contentDirectory = ServiceProxy::wrap(GUPNP_SERVICE_PROXY(info));
    }
}

void UPnPMediaServer::browse(const QString &id)
{
    BrowseModelStack::getDefault().push(new BrowseModel(m_contentDirectory, id));
}
