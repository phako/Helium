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

#ifndef UPNPMEDIASERVER_H
#define UPNPMEDIASERVER_H

#include <libgupnp-av/gupnp-av.h>

#include <QObject>
#include <QAbstractListModel>

#include "upnpdevice.h"
#include "browsemodel.h"

class UPnPMediaServer : public UPnPDevice
{
    Q_OBJECT
    Q_ENUMS(SortOrder)
public:
    enum SortOrder {
        SORT_DEFAULT,
        SORT_MUSIC_ALUBM
    };

    static const char DEVICE_TYPE[];
    static const char CONTENT_DIRECTORY_SERVICE[];

    explicit UPnPMediaServer();
    Q_INVOKABLE virtual void wrapDevice(const QString &udn);
    Q_INVOKABLE void browse(const QString &id = QLatin1String("0"), const QString &upnpClass = QLatin1String("object.container"));

Q_SIGNALS:
    void ready();

public Q_SLOTS:

private:
    ServiceProxy              m_contentDirectory;
    ServiceProxy              m_connectionManager;
    QString                   m_protocolInfo;
    QHash<SortOrder, QString> m_sortCriteria;

    static void on_get_protocol_info(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer user_data);
    static void on_get_sort_capabilities(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer user_data);

    bool isReady();
    void setupSortCriterias(const QString &caps);
};

#endif // UPNPMEDIASERVER_H
