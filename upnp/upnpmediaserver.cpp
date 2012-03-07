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

#include "upnpmediaserver.h"
#include "browsemodel.h"

#include <QDebug>
#include <QtDeclarative>

#include "browsemodelstack.h"

const char UPnPMediaServer::DEVICE_TYPE[] = "urn:schemas-upnp-org:device:MediaServer:";
const char UPnPMediaServer::CONTENT_DIRECTORY_SERVICE[] = "urn:schemas-upnp-org:service:ContentDirectory";


struct BrowseTask {
    QString                     containerId;
    UPnPMediaServer::SortOrder  sortOrder;
    QString                     protocolInfo;
};

UPnPMediaServer::UPnPMediaServer()
    : UPnPDevice()
    , m_contentDirectory()
    , m_connectionManager()
    , m_protocolInfo()
    , m_sortCriteria()
    , m_tasks()
{
}

UPnPMediaServer::~UPnPMediaServer()
{
    qDeleteAll(m_tasks);
}

void UPnPMediaServer::on_get_protocol_info(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer user_data)
{
    UPnPMediaServer *self = reinterpret_cast<UPnPMediaServer*>(user_data);
    char *protocol_info = 0;
    GError *error = 0;

    gupnp_service_proxy_end_action(proxy,
                                   action,
                                   &error,
                                   "Source", G_TYPE_STRING, &protocol_info,
                                   NULL);
    if (error != 0) {
        qWarning() << "Failed to get sort capabilites:" << error->message;
        g_error_free(error);

        self->m_protocolInfo = QLatin1String("");
    } else {
        self->m_protocolInfo = QString::fromUtf8(protocol_info);
        g_free(protocol_info);
    }

    if (self->isReady()) {
        // QTBUG-24571
        QMetaObject::invokeMethod(self, "ready", Qt::QueuedConnection);
    }
}

bool UPnPMediaServer::isReady()
{
    return (not m_sortCriteria.isEmpty() && not m_protocolInfo.isNull());
}

void UPnPMediaServer::on_get_sort_capabilities(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer user_data)
{
    UPnPMediaServer *self = reinterpret_cast<UPnPMediaServer*>(user_data);
    char *sort_caps;
    GError *error = 0;

    gupnp_service_proxy_end_action(proxy,
                                   action,
                                   &error,
                                   "SortCaps", G_TYPE_STRING, &sort_caps,
                                   NULL);
    if (error != 0) {
        qWarning() << "Failed to get sort capabilites:" << error->message;
        g_error_free(error);

        self->setupSortCriterias(QLatin1String(""));
    } else {
        self->setupSortCriterias(QString::fromUtf8(sort_caps));

        g_free (sort_caps);
    }

    if (self->isReady()) {
        // QTBUG-24571
        QMetaObject::invokeMethod(self, "ready", Qt::QueuedConnection);
    }
}

void UPnPMediaServer::setupSortCriterias(const QString &caps)
{
    QStringList sortCaps = caps.split(QLatin1Char(','));
    QStringList sortCriteria;

    int titleIndex = sortCaps.indexOf(QLatin1String("dc:title"));
    int classIndex = sortCaps.indexOf(QLatin1String("upnp:class"));
    int trackNrIndex = sortCaps.indexOf(QLatin1String("upnp:originalTrackNumber"));
    if (classIndex >= 0) {
        sortCriteria << sortCaps.at(classIndex);
    }
    if (titleIndex >= 0) {
        sortCriteria << sortCaps.at(titleIndex);
    }
    m_sortCriteria[SORT_DEFAULT] = sortCriteria.replaceInStrings(QRegExp(QLatin1String("^")), QLatin1String("+")).join(QLatin1String(","));
    sortCriteria.clear();

    if (trackNrIndex >= 0) {
        sortCriteria << sortCaps.at(trackNrIndex);
    }
    if (titleIndex >= 0) {
        sortCriteria << sortCaps.at(titleIndex);
    }
    m_sortCriteria[SORT_MUSIC_ALUBM] = sortCriteria.replaceInStrings(QRegExp(QLatin1String("^")), QLatin1String("+")).join(QLatin1String(","));
}

void UPnPMediaServer::wrapDevice(const QString &udn)
{
    UPnPDevice::wrapDevice(udn);
    m_contentDirectory = getService(UPnPMediaServer::CONTENT_DIRECTORY_SERVICE);
    m_connectionManager = getService(UPnPDevice::CONNECTION_MANAGER_SERVICE);

    // Get information on the device we need later on
    if (not m_connectionManager.isEmpty()) {
        gupnp_service_proxy_begin_action(m_connectionManager,
                                         "GetProtocolInfo",
                                         UPnPMediaServer::on_get_protocol_info,
                                         this,
                                         NULL);
    }

    if (not m_contentDirectory.isEmpty()) {
        gupnp_service_proxy_begin_action(m_contentDirectory,
                                         "GetSortCapabilities",
                                         UPnPMediaServer::on_get_sort_capabilities,
                                         this,
                                         NULL);
    }
}

void UPnPMediaServer::browse(const QString &id, const QString &upnpClass, const QString &protocolInfo)
{
    SortOrder sortOrder = SORT_DEFAULT;
    if (upnpClass.startsWith(QLatin1String("object.container.album.musicAlbum"))) {
        sortOrder = SORT_MUSIC_ALUBM;
    }

    BrowseTask *task = new BrowseTask;
    task->containerId = id;
    task->sortOrder = sortOrder;
    task->protocolInfo = protocolInfo;
    m_tasks << task;

    // Browse empty model to trigger busy indicator
    BrowseModelStack::getDefault().push(&BrowseModel::empty());
    if (isReady()) {
        QTimer::singleShot(0, this, SLOT(startBrowsing()));
    } else {
        connect(this, SIGNAL(ready()), SLOT(startBrowsing()));
    }
}

void UPnPMediaServer::startBrowsing()
{
    disconnect(this, SLOT(startBrowsing()));

    qDebug() << "startBrowsing called";
    QScopedPointer<BrowseTask> task(m_tasks.takeFirst());

    // and remove that model again
    BrowseModelStack::getDefault().pop();
    BrowseModel *model = new BrowseModel(m_contentDirectory,
                                         task->containerId,
                                         m_sortCriteria[task->sortOrder],
                                         task->protocolInfo);
    BrowseModelStack::getDefault().push(model);
    QTimer::singleShot(0, model, SLOT(onStartBrowse()));
    connect(model,SIGNAL(error(int, QString)), SIGNAL(error(int,QString)));

    // not very likely, but let's check it anyway
    if (m_tasks.count() > 0) {
        QTimer::singleShot(0, this, SLOT(startBrowsing()));
    }
}
