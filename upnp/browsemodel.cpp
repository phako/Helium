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

#include <QDebug>
#include <QTimer>
#include <QUrl>

#include "browsemodel.h"
#include "upnpdevicemodel.h"

const int BROWSE_SLICE = 100;
const char DEFAULT_FILTER[] = "res@size,res@duration,dc:title,upnp:class,@id,upnp:albumArtURI";
const char AUDIO_PREFIX[] = "object.item.audioItem";
const char IMAGE_PREFIX[] = "object.item.imageItem";
const char VIDEO_PREFIX[] = "object.item.videoItem";

BrowseModel::BrowseModel(const ServiceProxy &proxy,
                         const QString      &id,
                         QObject            *parent)
    : QAbstractListModel(parent)
    , m_contentDirectory(proxy)
    , m_id(id)
    , m_currentOffset(0)
    , m_busy(true)
{
    QHash<int, QByteArray> roles;

    roles[BrowseRoleTitle] = "title";
    roles[BrowseRoleId] = "upnpId";
    roles[BrowseRoleUPnPClass] = "upnpClass";
    roles[BrowseRoleDuration] = "duration";
    roles[BrowseRoleSize] = "size";
    roles[BrowseRoleIcon] = "icon";
    roles[BrowseRoleType] = "type";
    setRoleNames(roles);

    qDebug() << "Created browse model";

    // go through event loop
    if (!proxy.isEmpty()) {
        QTimer::singleShot(0, this, SLOT(onStartBrowse()));
    }
}

int BrowseModel::rowCount(const QModelIndex &/*parent*/) const
{
    return m_data.count();
}


static QUrl findIconForObject(GUPnPDIDLLiteObject *object)
{
    QUrl thumbnail;
    GList* resources = gupnp_didl_lite_object_get_resources(object);
    GList* it = resources;

    const char *upnp_class = gupnp_didl_lite_object_get_upnp_class (object);

    if (strncmp(upnp_class, AUDIO_PREFIX, sizeof(AUDIO_PREFIX)) == 0) {
        const char *album_art_uri = gupnp_didl_lite_object_get_album_art(object);
        thumbnail.setUrl(QString::fromUtf8(album_art_uri));
    }

    while (it) {
        GUPnPDIDLLiteResource *res = (GUPnPDIDLLiteResource*) it->data;
        GUPnPProtocolInfo *info = gupnp_didl_lite_resource_get_protocol_info(res);
        const char *profile = gupnp_protocol_info_get_dlna_profile(info);
        if (profile != 0 &&
            (strncmp(profile, "JPEG_TN", 7) == 0 ||
             strncmp(profile, "PNG_TN", 6) == 0)) {
            thumbnail.setUrl(QString::fromUtf8(gupnp_didl_lite_resource_get_uri(res)));
            break;
        }
        it = it->next;
    }

    if (thumbnail.isEmpty()) {
        if (strncmp(upnp_class, IMAGE_PREFIX, sizeof(IMAGE_PREFIX)) == 0) {
            thumbnail.setUrl("image://theme/icon-m-content-image-inverse");
        } else if (strncmp(upnp_class, VIDEO_PREFIX, sizeof(VIDEO_PREFIX)) == 0) {
            thumbnail.setUrl("image://theme/icon-m-content-videos-inverse");
        } else if (strncmp(upnp_class, AUDIO_PREFIX, sizeof(AUDIO_PREFIX)) == 0) {
            thumbnail.setUrl("image://theme/icon-m-content-audio-inverse");
        }
    }

    g_list_free_full(resources, g_object_unref);

    return thumbnail;
}

static QUrl findUriForObject(GUPnPDIDLLiteObject *object)
{
    QUrl url;

    GList* resources = gupnp_didl_lite_object_get_resources(object);
    GList* it = resources;

    while (it) {
        GUPnPDIDLLiteResource *res = (GUPnPDIDLLiteResource*) it->data;
        GUPnPProtocolInfo *info = gupnp_didl_lite_resource_get_protocol_info(res);
        // use first non-transcoded uri; might be problematic
        if (gupnp_protocol_info_get_dlna_conversion (info) == GUPNP_DLNA_CONVERSION_NONE) {
            url.setUrl(QString::fromUtf8(gupnp_didl_lite_resource_get_uri(res)));

            break;
        }
        it = it->next;
    }

    g_list_free_full(resources, g_object_unref);

    return url;
}

QVariant BrowseModel::data(const QModelIndex &index, int role) const
{
    qDebug() << "Trying to get data for" << index.row() << "and role" << role;
    if (!index.isValid()) {
        return QVariant();
    }

    DIDLLiteObject object = m_data.at(index.row());
    if (object.isEmpty()) {
        return QVariant();
    }

    switch (role) {
    case BrowseRoleTitle:
        return QString::fromUtf8(gupnp_didl_lite_object_get_title(object));
    case BrowseRoleId:
        return QString::fromUtf8(gupnp_didl_lite_object_get_id(object));
    case BrowseRoleUPnPClass:
        return QString::fromUtf8(gupnp_didl_lite_object_get_upnp_class(object));
    case BrowseRoleIcon:
        if (GUPNP_IS_DIDL_LITE_CONTAINER(object)) {
            return QUrl("image://theme/icon-l-folder-empty");
        } else {
            return findIconForObject(object);
        }
    case BrowseRoleURI:
        if (GUPNP_IS_DIDL_LITE_ITEM(object)) {
            return findUriForObject(object);
        }
        return QVariant();
    case BrowseRoleType:
        if (GUPNP_IS_DIDL_LITE_CONTAINER(object)) {
            return QLatin1String("container");
        } else {
            return QLatin1String("object");
        }
    default:
        return QVariant();
    }
}

void BrowseModel::on_didl_object (GUPnPDIDLLiteParser *parser,
                                  GUPnPDIDLLiteObject *object,
                                  gpointer             user_data)
{
    qDebug () << "On didl-item";
    Q_UNUSED(parser)

    BrowseModel *model = reinterpret_cast<BrowseModel *>(user_data);
    model->beginInsertRows(QModelIndex(), model->m_data.count(), model->m_data.count());
    model->m_data.append(DIDLLiteObject(object));
    model->endInsertRows();
}

void BrowseModel::onStartBrowse()
{
    qDebug () << "Starting to browse" << m_id;
    gupnp_service_proxy_begin_action(m_contentDirectory,
                                     "Browse",
                                     BrowseModel::on_browse,
                                     this,
                                     "ObjectID", G_TYPE_STRING, m_id.toUtf8().constData(),
                                     "BrowseFlag", G_TYPE_STRING, "BrowseDirectChildren",
                                     "Filter", G_TYPE_STRING, "*", //DEFAULT_FILTER,
                                     "StartingIndex", G_TYPE_UINT, 0, //m_currentOffset,
                                     "RequestedCount", G_TYPE_UINT, 0, //BROWSE_SLICE,
                                     "SortCriteria", G_TYPE_STRING, "+dc:title",
                                     NULL);
}

void BrowseModel::on_browse(GUPnPServiceProxy       *proxy,
                            GUPnPServiceProxyAction *action,
                            gpointer                 user_data)
{
    GError *error = 0;
    guint number_returned;
    guint total_matches;
    char *result;
    BrowseModel *model = reinterpret_cast<BrowseModel *>(user_data);

    model->setBusy(false);

    gupnp_service_proxy_end_action(proxy,
                                   action,
                                   &error,
                                   "Result", G_TYPE_STRING, &result,
                                   "NumberReturned", G_TYPE_UINT, &number_returned,
                                   "TotalMatches", G_TYPE_UINT, &total_matches,
                                   NULL);
    if (error != 0) {
        qDebug() << "Browsing failed:" << error->message;
        g_error_free(error);
        return;
    }
    RefPtrG<GUPnPDIDLLiteParser> parser = RefPtrG<GUPnPDIDLLiteParser>::wrap(gupnp_didl_lite_parser_new());
/*    model->beginInsertRows(QModelIndex(),
                           model->m_data.count(),
                           model->m_data.count() + number_returned - 1); */
    qDebug() << "beginInsert" << model->m_data.count() << model->m_data.count() + number_returned - 1;
    g_signal_connect (G_OBJECT(parser),
                      "object-available",
                      G_CALLBACK(BrowseModel::on_didl_object),
                      user_data);

    qDebug() << "before parse";
    gupnp_didl_lite_parser_parse_didl(parser, result, &error);
    qDebug() << "after parse";

//    model->endInsertRows();
    qDebug() << "afterInsertRows";
    model->dataChanged(model->index(0), model->index(model->rowCount()));

    if (result != 0) {
        g_free (result);
    }
}
