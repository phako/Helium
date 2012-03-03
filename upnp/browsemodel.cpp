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
#include <QStringList>
#include <QTimer>
#include <QUrl>

#include "browsemodel.h"
#include "upnpdevicemodel.h"

const int BROWSE_SLICE = 100;
const char DEFAULT_FILTER[] = "res@size,res@duration,dc:title,upnp:class,@id,upnp:albumArtURI,upnp:artist,upnp:album";
const char AUDIO_PREFIX[] = "object.item.audioItem";
const char IMAGE_PREFIX[] = "object.item.imageItem";
const char VIDEO_PREFIX[] = "object.item.videoItem";

BrowseModel BrowseModel::m_empty;

BrowseModel::BrowseModel(const ServiceProxy &proxy,
                         const QString      &id,
                         const QString      &sortCriteria,
                         QObject            *parent)
    : QAbstractListModel(parent)
    , m_contentDirectory(proxy)
    , m_id(id)
    , m_currentOffset(0)
    , m_busy(true)
    , m_done(false)
    , m_action(0)
    , m_sortCriteria(sortCriteria)
{
    QHash<int, QByteArray> roles;

    roles[BrowseRoleTitle] = "title";
    roles[BrowseRoleId] = "upnpId";
    roles[BrowseRoleUPnPClass] = "upnpClass";
    roles[BrowseRoleIcon] = "icon";
    roles[BrowseRoleType] = "type";
    roles[BrowseRoleDetail] = "detail";
    roles[BrowseRoleURI] = "uri";
    setRoleNames(roles);

    qDebug() << "Created browse model";
}

BrowseModel::~BrowseModel()
{
    if (m_action != 0) {
        gupnp_service_proxy_cancel_action (m_contentDirectory, m_action);
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

    if (strncmp(upnp_class, AUDIO_PREFIX, sizeof(AUDIO_PREFIX) - 1) == 0) {
        const char *album_art_uri = gupnp_didl_lite_object_get_album_art(object);
        thumbnail.setUrl(QString::fromUtf8(album_art_uri));
    } else {
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
    }

    if (thumbnail.isEmpty()) {
        if (strncmp(upnp_class, IMAGE_PREFIX, sizeof(IMAGE_PREFIX) - 1) == 0) {
            thumbnail.setUrl("image://theme/icon-m-content-image-inverse");
        } else if (strncmp(upnp_class, VIDEO_PREFIX, sizeof(VIDEO_PREFIX) - 1) == 0) {
            thumbnail.setUrl("image://theme/icon-m-content-videos-inverse");
        } else if (strncmp(upnp_class, AUDIO_PREFIX, sizeof(AUDIO_PREFIX) - 1) == 0) {
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

static QString createDetailsForObject(GUPnPDIDLLiteObject *object)
{
    QString result;
    QStringList sizes = QStringList() << QLatin1String("GiB")
                                      << QLatin1String("MiB")
                                      << QLatin1String("KiB")
                                      << QLatin1String("bytes");

    GList* resources = gupnp_didl_lite_object_get_resources(object);
    GList* it = resources;

    const char *author = gupnp_didl_lite_object_get_artist(object);
    const char *album = gupnp_didl_lite_object_get_album(object);

    if (author != 0) {
        result += QString::fromLatin1("by %1").arg(QString::fromUtf8(author));
    }

    if (album != 0) {
        if (not result.isEmpty()) {
            result += QLatin1String(" ");
        }
        result += QString::fromLatin1("on %1").arg(QString::fromUtf8(album));
    }

    while (it) {
        GUPnPDIDLLiteResource *res = (GUPnPDIDLLiteResource*) it->data;
        GUPnPProtocolInfo *info = gupnp_didl_lite_resource_get_protocol_info(res);
        // use first non-transcoded uri; might be problematic
        if (gupnp_protocol_info_get_dlna_conversion (info) == GUPNP_DLNA_CONVERSION_NONE) {
            long duration = gupnp_didl_lite_resource_get_duration(res);
            gint64 size = gupnp_didl_lite_resource_get_size64(res);
            if (duration > 0) {
                int hours = duration / 60 / 60;
                duration %= 3600;
                int minutes = duration / 60;
                duration %= 60;
                qDebug() << hours << minutes << duration;
                if ((hours > 0 || minutes > 0 || duration > 0) &&
                    not result.isEmpty()) {
                    result += QLatin1String(" ");
                }
                if (hours > 0) {
                    result += QString::fromLatin1("%1:").arg(hours);
                }
                if (minutes > 0 || hours > 0 || duration > 0) {
                    result += QString::fromLatin1("%1:").arg((int) minutes, hours > 0 || minutes > 9 ? 2 : 1, 10, QLatin1Char('0'));
                }
                if (duration > 0 || hours > 0 || minutes > 0) {
                    result += QString::fromLatin1("%1").arg((int) duration, 2, 10, QLatin1Char('0'));
                }

                if (size > 0) {
                    while (size > 1024 && sizes.last() != QLatin1String("GiB")) {
                        size >>= 10;
                        sizes.removeLast();
                    }
                    if (not result.isEmpty()) {
                        result += " ";
                    }
                    result += QString::fromLatin1("%1%2").arg(QString::number(size), sizes.last());
                }
            }

            break;
        }
        it = it->next;
    }


    g_list_free_full(resources, g_object_unref);
    return result;
}

QVariant BrowseModel::data(const QModelIndex &index, int role) const
{
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
    case BrowseRoleDetail:
        if (GUPNP_IS_DIDL_LITE_CONTAINER(object)) {
            return QString();
        }
        return createDetailsForObject(object);
    default:
        return QVariant();
    }
}

void BrowseModel::on_didl_object (GUPnPDIDLLiteParser *parser,
                                  GUPnPDIDLLiteObject *object,
                                  gpointer             user_data)
{
    Q_UNUSED(parser)

    BrowseModel *model = reinterpret_cast<BrowseModel *>(user_data);
    model->m_data.append(DIDLLiteObject(object));
}

void BrowseModel::onStartBrowse()
{
    if (m_contentDirectory.isEmpty()) {
        return;
    }

    qDebug () << "Starting to browse" << m_id;
    m_action = gupnp_service_proxy_begin_action(m_contentDirectory,
                                                "Browse",
                                                BrowseModel::on_browse,
                                                this,
                                                "ObjectID", G_TYPE_STRING, m_id.toUtf8().constData(),
                                                "BrowseFlag", G_TYPE_STRING, "BrowseDirectChildren",
                                                "Filter", G_TYPE_STRING, DEFAULT_FILTER,
                                                "StartingIndex", G_TYPE_UINT, m_currentOffset,
                                                "RequestedCount", G_TYPE_UINT, BROWSE_SLICE,
                                                "SortCriteria", G_TYPE_STRING, m_sortCriteria.toUtf8().constData(),
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

    gupnp_service_proxy_end_action(proxy,
                                   action,
                                   &error,
                                   "Result", G_TYPE_STRING, &result,
                                   "NumberReturned", G_TYPE_UINT, &number_returned,
                                   "TotalMatches", G_TYPE_UINT, &total_matches,
                                   NULL);
    QMetaObject::invokeMethod(model, "setBusy", Qt::QueuedConnection,
                              Q_ARG(bool, false));
    model->m_action = 0;
    if (error != 0) {
        qDebug() << "Browsing failed:" << error->message;
        g_error_free(error);
        if (result != 0) {
            g_free (result);
        }

        return;
    }

    if (number_returned == 0) {
        if (result != 0) {
            g_free(result);
        }
        return;
    }

    QMetaObject::invokeMethod(model, "onBrowseDone", Qt::QueuedConnection,
                              Q_ARG(QByteArray, QByteArray::fromRawData(result, strlen(result))),
                              Q_ARG(uint, number_returned),
                              Q_ARG(uint, total_matches));
}

void BrowseModel::onBrowseDone(QByteArray result, uint number_returned, uint total_matches)
{
    RefPtrG<GUPnPDIDLLiteParser> parser = RefPtrG<GUPnPDIDLLiteParser>::wrap(gupnp_didl_lite_parser_new());
    beginInsertRows(QModelIndex(),
                           m_data.count(),
                           m_data.count() + number_returned - 1);
    g_signal_connect (G_OBJECT(parser),
                      "object-available",
                      G_CALLBACK(BrowseModel::on_didl_object),
                      this);

    GError *error = 0;
    gupnp_didl_lite_parser_parse_didl(parser, result.constData(), &error);

    endInsertRows();

    if (not result.isNull()) {
        g_free(const_cast<char *>(result.constData()));
        result.setRawData(0, 0);
    }

    m_currentOffset += number_returned;
    if (total_matches > 0 && m_currentOffset < total_matches) {
        QTimer::singleShot(0, this, SLOT(onStartBrowse()));
    } else {
        setDone(true);
    }
}

void BrowseModel::refresh() {
    beginResetModel();
    setDone(false);
    setBusy(true);
    m_currentOffset = 0;
    m_data.clear();
    QTimer::singleShot(0, this, SLOT(onStartBrowse()));
    endResetModel();
}
