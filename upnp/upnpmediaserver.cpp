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
const QLatin1String MUSIC_ALBUM_CLASS = QLatin1String("object.container.album.musicAlbum");

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
    , m_pendingCalls()
{
}

UPnPMediaServer::~UPnPMediaServer()
{
    qDeleteAll(m_tasks);
}

void UPnPMediaServer::onGetProtocolInfo()
{
    ServiceProxyCall *call = qobject_cast<ServiceProxyCall *>(sender());

    if (call == 0) {
        return;
    }

    call->deleteLater();
    m_pendingCalls.removeOne(call);
    call->finalize(QStringList() << QLatin1String("Source"));
    if (call->hasError()) {
        Q_EMIT error(call->errorCode(), call->errorMessage());
        m_protocolInfo = QLatin1String("");

        return;
    }

    m_protocolInfo = call->get(QLatin1String("Source")).toString();

    if (isReady()) {
        Q_EMIT ready();
    }
}

bool UPnPMediaServer::isReady()
{
    return (not m_sortCriteria.isEmpty() && not m_protocolInfo.isNull());
}

void UPnPMediaServer::onGetSortCapabilities()
{
    ServiceProxyCall *call = qobject_cast<ServiceProxyCall *>(sender());
    if (call == 0) {
        return;
    }

    m_pendingCalls.removeOne(call);
    call->deleteLater();
    call->finalize(QStringList() << QLatin1String("SortCaps"));
    if (call->hasError()) {
        qDebug() << "Failed to retrieve sort caps" << call->errorMessage();
        setupSortCriterias(QLatin1String(""));

        return;
    }

    setupSortCriterias(call->get(QLatin1String("SortCaps")).toString());
    if (isReady()) {
        Q_EMIT ready();
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
    m_contentDirectory.reset(getService(UPnPMediaServer::CONTENT_DIRECTORY_SERVICE));
    m_connectionManager.reset(getService(UPnPDevice::CONNECTION_MANAGER_SERVICE));

    // Get information on the device we need later on
    if (not m_connectionManager.isNull() && not m_connectionManager->isNull()) {
        m_pendingCalls << m_connectionManager->call(QLatin1String("GetProtocolInfo"));
        connect(m_pendingCalls.last(), SIGNAL(ready()), SLOT(onGetProtocolInfo()));
        m_pendingCalls.last()->run();
    }

    if (m_contentDirectory && not m_contentDirectory->isNull()) {
        m_pendingCalls << m_contentDirectory->call(QLatin1String("GetSortCapabilities"));
        connect(m_pendingCalls.last(), SIGNAL(ready()), SLOT(onGetSortCapabilities()));
        m_pendingCalls.last()->run();
    }
}

void UPnPMediaServer::browse(const QString &id, const QString &upnpClass, const QString &protocolInfo)
{
    SortOrder sortOrder = SORT_DEFAULT;
    if (upnpClass.startsWith(MUSIC_ALBUM_CLASS)) {
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
