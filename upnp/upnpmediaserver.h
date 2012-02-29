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

#ifndef UPNPMEDIASERVER_H
#define UPNPMEDIASERVER_H

#include <libgupnp-av/gupnp-av.h>

#include <QObject>
#include <QAbstractListModel>

#include "upnpdevice.h"

class UPnPMediaServer : public UPnPDevice
{
    Q_OBJECT
public:
    static const char DEVICE_TYPE[];
    static const char CONTENT_DIRECTORY_SERVICE[];

    explicit UPnPMediaServer();
    Q_INVOKABLE virtual void wrapDevice(const QString &udn);

Q_SIGNALS:
    
public Q_SLOTS:
    void browse(const QString &id = QLatin1String("0"));

private:
    ServiceProxy m_contentDirectory;
};

#endif // UPNPMEDIASERVER_H
