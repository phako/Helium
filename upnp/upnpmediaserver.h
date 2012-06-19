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

#include <memory>

#include <QObject>
#include <QtCore/QScopedPointer>

#include "browsemodel.h"
#include "upnpdevice.h"
#include "serviceproxycall.h"

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
    ~UPnPMediaServer();

    Q_INVOKABLE virtual void wrapDevice(const QString &udn);
    Q_INVOKABLE void browse(const QString &id = QLatin1String("0"),
                            const QString &upnpClass = QLatin1String("object.container"),
                            const QString &protocolInfo = QLatin1String(""));

private Q_SLOTS:
    void startBrowsing();
    void onGetSortCapabilities();
    void onGetProtocolInfo();
private:
    std::shared_ptr<ServiceProxy> m_contentDirectory;
    QScopedPointer<ServiceProxy>  m_connectionManager;
    QString                   m_protocolInfo;
    QHash<SortOrder, QString> m_sortCriteria;
    QList<BrowseModel*>        m_tasks;

    bool isReady();
    void setupSortCriterias(const QString &caps);
};

#endif // UPNPMEDIASERVER_H
