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

#include <cmath>

#include <QDebug>
#include <QStringList>

#include "upnprenderer.h"

struct SetAVTransportCall {
    QString m_uri;
    QString m_metaData;
    UPnPRenderer *m_renderer;

    SetAVTransportCall(const QString& uri, const QString &metaData, UPnPRenderer *renderer)
        : m_uri(uri)
        , m_metaData(metaData)
        , m_renderer(renderer) { }
};

static quint64 parseDurationString(const QString& duration)
{
    quint64 seconds = 0;
    int factor = 1;

    int length = duration.indexOf(QLatin1Char('.')) > 0 ?: duration.length();
    QStringList list = duration.left(length).split(QLatin1Char(':'));
    if (list.count() > 3) {
        return 0;
    }

    for (int i = list.count() - 1; i >= 0; i--) {
        seconds += list[i].toInt() * factor;
        factor *= 60;
    }

    return seconds;
}

const char UPnPRenderer::DEVICE_TYPE[] = "urn:schemas-upnp-org:device:MediaRenderer:";
const char UPnPRenderer::AV_TRANSPORT_SERVICE[] = "urn:schemas-upnp-org:service:AVTransport";

void UPnPRenderer::setState(const QString &state)
{
    qDebug () << "New state" << state;
    m_state = state;
    if (state == QLatin1String("PLAYING")) {
        m_progressTimer.start(1000);
    } else {
        m_progressTimer.stop();
    }
    QMetaObject::invokeMethod(this, "stateChanged", Qt::QueuedConnection);
}

void UPnPRenderer::setDuration(const QString& trackDuration)
{
    if (trackDuration == m_duration) {
        return;
    }

    m_duration = trackDuration;
    m_durationInSeconds = parseDurationString(m_duration);

    QMetaObject::invokeMethod(this, "durationChanged", Qt::QueuedConnection);
}

void UPnPRenderer::setProgress(float progress)
{
    if (fabs(progress - m_progress) < 0.00001) {
        return;
    }

    m_progress = progress;

    QMetaObject::invokeMethod(this, "progressChanged", Qt::QueuedConnection);
}

void UPnPRenderer::setProtocolInfo(const QString &protocolInfo)
{
    if (m_protocolInfo == protocolInfo) {
        return;
    }

    m_protocolInfo = protocolInfo;

    QMetaObject::invokeMethod(this, "protocolInfoChanged", Qt::QueuedConnection);
}

void UPnPRenderer::setURI(const QString& uri)
{
    if (uri == m_uri) {
        return;
    }

    m_uri = uri;

    QMetaObject::invokeMethod(this, "uriChanged", Qt::QueuedConnection);
}

void UPnPRenderer::setCanPause(bool canPause)
{
    if (canPause == this->m_canPause) {
        return;
    }

    m_canPause = canPause;

    QMetaObject::invokeMethod(this, "canPauseChanged", Qt::QueuedConnection);
}

void UPnPRenderer::setPosition(const QString &position)
{
    if (position == m_position) {
        return;
    }

    m_position = position;

    QMetaObject::invokeMethod(this, "positionChanged", Qt::QueuedConnection);
}

void UPnPRenderer::setCanSeek(bool canSeek)
{
    if (canSeek == m_canSeek) {
        return;
    }

    m_canSeek = canSeek;

    QMetaObject::invokeMethod(this, "canSeekChanged", Qt::QueuedConnection);
}

void UPnPRenderer::setSeekMode(const QString &seekMode)
{
    if (seekMode == m_seekMode) {
        return;
    }

    m_seekMode = seekMode;

    QMetaObject::invokeMethod(this, "seekModeChanged", Qt::QueuedConnection);
}

void
UPnPRenderer::on_transport_state_changed (GUPnPServiceProxy */*service*/,
                                          const char        */*variable*/,
                                          GValue            *value,
                                          gpointer           user_data)
{
    UPnPRenderer *renderer = reinterpret_cast<UPnPRenderer*>(user_data);
    GError *error = 0;
    char *state_name = 0;
    char *track_duration = 0;
    char *track_uri = 0;

    if (gupnp_last_change_parser_parse_last_change(renderer->m_lastChangeParser,
                                                   0,
                                                   g_value_get_string(value),
                                                   &error,
                                                   "TransportState", G_TYPE_STRING, &state_name,
                                                   "CurrentTrackDuration", G_TYPE_STRING, &track_duration,
                                                   "CurrentTrackURI", G_TYPE_STRING, &track_uri,
                                                   NULL)) {
        if (state_name != 0) {
            renderer->setState(QString::fromUtf8(state_name));

            g_free(state_name);
        }

        if (track_duration != 0) {
            renderer->setDuration(QString::fromUtf8(track_duration));

            g_free(track_duration);
        }

        if (track_uri != 0) {
            renderer->setURI(QString::fromUtf8(track_uri));

            g_free(track_uri);
        }
    } else {
        qDebug() << "Failed to parse last change" << error->message;
        g_error_free(error);
    }
}

UPnPRenderer::UPnPRenderer()
    : UPnPDevice()
    , m_lastChangeParser(RefPtrG<GUPnPLastChangeParser>::wrap(gupnp_last_change_parser_new()))
    , m_avTransport()
    , m_connectionManager()
    , m_state(QLatin1String("STOPPED"))
    , m_protocolInfo(QLatin1String("*:*:*:*"))
    , m_duration(QLatin1String("0:00:00"))
    , m_progressTimer()
    , m_canPause(false)
    , m_uri()
    , m_position(QLatin1String("0:00:00"))
    , m_canSeek(false)
    , m_seekMode(QLatin1String(""))
{
    connect(&m_progressTimer, SIGNAL(timeout()), SLOT(onProgressTimeout()));
}

void UPnPRenderer::onProgressTimeout()
{
    gupnp_service_proxy_begin_action(m_avTransport,
                                     "GetPositionInfo",
                                     UPnPRenderer::on_get_position_info,
                                     this,
                                     "InstanceID", G_TYPE_UINT, 0,
                                     NULL);
}

void UPnPRenderer::unsubscribe()
{
    gupnp_service_proxy_set_subscribed(m_avTransport, FALSE);
    gupnp_service_proxy_remove_notify(m_avTransport,
                                      "LastChange",
                                      UPnPRenderer::on_transport_state_changed,
                                      this);
}

UPnPRenderer::~UPnPRenderer()
{
    unsubscribe();
}

void UPnPRenderer::wrapDevice(const QString &udn)
{
    // check if we are already wrapping a device and remove the signals
    if (!m_proxy.isEmpty()) {
        unsubscribe();
    }

    UPnPDevice::wrapDevice(udn);

    // reset to initial state
    setState(QLatin1String("STOPPED"));
    setProtocolInfo(QLatin1String("*:*:*:*"));
    setDuration(QLatin1String("0:00:00"));
    setCanPause(false);
    setURI(QString());
    setPosition(QLatin1String("0:00:00"));
    setProgress(0.0f);

    if (m_proxy.isEmpty()) {
        return;
    }


    m_avTransport = getService(UPnPRenderer::AV_TRANSPORT_SERVICE);
    m_connectionManager = getService(UPnPDevice::CONNECTION_MANAGER_SERVICE);
    gupnp_service_proxy_add_notify(m_avTransport,
                                   "LastChange", G_TYPE_STRING,
                                   UPnPRenderer::on_transport_state_changed,
                                   this);

    gupnp_service_proxy_set_subscribed(m_avTransport, TRUE);
    gupnp_service_proxy_begin_action(m_connectionManager,
                                     "GetProtocolInfo",
                                     UPnPRenderer::on_get_protocol_info,
                                     this,
                                     NULL);
}

void UPnPRenderer::on_get_position_info(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer user_data)
{
    UPnPRenderer *self = reinterpret_cast<UPnPRenderer*>(user_data);
    GError *error = 0;
    char *rel_time = 0;

    gupnp_service_proxy_end_action(proxy,
                                   action,
                                   &error,
                                   "RelTime", G_TYPE_STRING, &rel_time,
                                   NULL);

    if (error != 0) {
        QMetaObject::invokeMethod(self, "error",
                                  Qt::QueuedConnection,
                                  Q_ARG(int, error->code),
                                  Q_ARG(QString, QString::fromUtf8(error->message)));
        g_error_free(error);

        return;
    }

    self->setPosition(QString::fromUtf8(rel_time));
    self->setProgress((double) parseDurationString(QString::fromUtf8(rel_time)) / (double) self->m_durationInSeconds);
}

void UPnPRenderer::on_get_protocol_info(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer user_data)
{
    UPnPRenderer *self = reinterpret_cast<UPnPRenderer*>(user_data);
    char *protocol_info = 0;
    GError *error = 0;

    gupnp_service_proxy_end_action(proxy,
                                   action,
                                   &error,
                                   "Sink", G_TYPE_STRING, &protocol_info,
                                   NULL);
    if (error != 0) {
        if (protocol_info != 0) {
            g_free(protocol_info);
        }

        protocol_info = 0;
        QMetaObject::invokeMethod(self, "error",
                                  Q_ARG(int, error->code),
                                  Q_ARG(QString, QString::fromUtf8(error->message)));
        g_error_free(error);
    }

    self->setProtocolInfo(QString::fromUtf8(protocol_info));
    gupnp_service_info_get_introspection_async(GUPNP_SERVICE_INFO(self->m_avTransport),
                                               UPnPRenderer::on_got_introspection,
                                               self);
    if (protocol_info != 0) {
        g_free(protocol_info);
    }
}

void UPnPRenderer::on_got_introspection (GUPnPServiceInfo *info,
                                         GUPnPServiceIntrospection *introspection,
                                         const GError *error,
                                         gpointer user_data)
{
    Q_UNUSED(info)

    UPnPRenderer *self = reinterpret_cast<UPnPRenderer*>(user_data);
    if (error != 0) {
        QMetaObject::invokeMethod(self, "error",
                                  Qt::QueuedConnection,
                                  Q_ARG(int, error->code),
                                  Q_ARG(QString, QString::fromUtf8(error->message)));
    } else {
        bool canPause = gupnp_service_introspection_get_action(introspection, "Pause") != NULL;
        self->setCanPause(canPause);

        // check supported seek types
        const GUPnPServiceStateVariableInfo *seekModeInfo =
            gupnp_service_introspection_get_state_variable(introspection, "A_ARG_TYPE_SeekMode");
        GList *it = seekModeInfo->allowed_values;
        while (it != 0) {
            if (strcmp ((const char *)it->data, "REL_TIME") == 0 ||
                strcmp ((const char *)it->data, "ABS_TIME") == 0) {
                self->setCanSeek(true);
                self->setSeekMode(QString::fromLatin1((const char *)it->data));

                break;
            }
            it = it->next;
        }

        g_object_unref(introspection);
    }

    QMetaObject::invokeMethod(self, "ready", Qt::QueuedConnection);
}

void UPnPRenderer::on_set_av_transport_uri (GUPnPServiceProxy       *proxy,
                                            GUPnPServiceProxyAction *action,
                                            gpointer                 user_data)
{
    QScopedPointer<SetAVTransportCall> call(static_cast<SetAVTransportCall*>(user_data));
    GError *error = 0;
    // needed to free the action
    gupnp_service_proxy_end_action(proxy,
                                   action,
                                   &error,
                                   NULL);
    if (error == 0) {
        return;
    }

    // "Transport locked". Call Stop() and try again.
    if (error->code == 705) {
        gupnp_service_proxy_begin_action(call->m_renderer->m_avTransport,
                                         "Stop",
                                         UPnPRenderer::on_stop,
                                         call.take(),
                                         "InstanceID", G_TYPE_STRING, "0",
                                         NULL);
        g_error_free (error);

        return;
    }

    QMetaObject::invokeMethod(call->m_renderer, "error",
                              Qt::QueuedConnection,
                              Q_ARG(int, error->code),
                              Q_ARG(QString, QString::fromUtf8(error->message)));

    g_error_free (error);
}

void UPnPRenderer::setAVTransportUri(const QString &uri, const QString &metaData)
{
    SetAVTransportCall *call = new SetAVTransportCall (uri, metaData, this);

    gupnp_service_proxy_begin_action(m_avTransport,
                                     "SetAVTransportURI",
                                     UPnPRenderer::on_set_av_transport_uri,
                                     call,
                                     "InstanceID", G_TYPE_STRING, "0",
                                     "CurrentURI", G_TYPE_STRING, uri.toUtf8().constData(),
                                     "CurrentURIMetaData", G_TYPE_STRING, metaData.toUtf8().constData(),
                                     NULL);
}

void UPnPRenderer::on_stop (GUPnPServiceProxy       *proxy,
                            GUPnPServiceProxyAction *action,
                            gpointer                 user_data)
{
    QScopedPointer<SetAVTransportCall> call(static_cast<SetAVTransportCall*>(user_data));
    GError *error = 0;

    // needed to free the action
    gupnp_service_proxy_end_action(proxy,
                                   action,
                                   &error,
                                   NULL);
    if (error != 0) {
        QMetaObject::invokeMethod(call->m_renderer, "error",
                                  Qt::QueuedConnection,
                                  Q_ARG(int, error->code),
                                  Q_ARG(QString, QString::fromUtf8(error->message)));

        g_error_free (error);

        return;
    }

    call->m_renderer->setAVTransportUri(call->m_uri, call->m_metaData);
}

void UPnPRenderer::on_play (GUPnPServiceProxy       *proxy,
                            GUPnPServiceProxyAction *action,
                            gpointer                 /*user_data*/)
{
    GError *error = 0;

    // needed to free the action
    gupnp_service_proxy_end_action(proxy,
                                   action,
                                   &error,
                                   NULL);
    if (error != 0) {
        g_error_free (error);
    }
}

void UPnPRenderer::play()
{
    if (m_state == QLatin1String("PLAYING")) {
        return;
    }

    gupnp_service_proxy_begin_action(m_avTransport,
                                     "Play",
                                     UPnPRenderer::on_play,
                                     this,
                                     "InstanceID", G_TYPE_STRING, "0",
                                     "Speed", G_TYPE_STRING, "1",
                                     NULL);
}

void UPnPRenderer::stop()
{
    if (m_state == QLatin1String("STOPPED")) {
        return;
    }

    gupnp_service_proxy_begin_action(m_avTransport,
                                     "Stop",
                                     UPnPRenderer::on_play,
                                     this,
                                     "InstanceID", G_TYPE_STRING, "0",
                                     NULL);
}

void UPnPRenderer::pause()
{
    if (m_state == QLatin1String("PAUSED_PLAYBACK")) {
        return;
    }

    gupnp_service_proxy_begin_action(m_avTransport,
                                     "Pause",
                                     UPnPRenderer::on_play,
                                     this,
                                     "InstanceID", G_TYPE_STRING, "0",
                                     NULL);
}

void UPnPRenderer::seekRelative(float percent)
{
    if (not canSeek()) {
        return;
    }

    quint64 position = percent * m_durationInSeconds;

    int hours = position / 3600;
    position %= 3600;
    int minutes = position / 60;
    int seconds = position % 60;

    QString target = QString::fromLatin1("%1:%2:%3").arg(hours)
                                                    .arg(minutes, 2, 10, QLatin1Char('0'))
                                                    .arg(seconds, 2, 10, QLatin1Char('0'));

    gupnp_service_proxy_begin_action(m_avTransport,
                                     "Seek",
                                     UPnPRenderer::on_play,
                                     this,
                                     "InstanceID", G_TYPE_STRING, "0",
                                     "Unit", G_TYPE_STRING, m_seekMode.toUtf8().constData(),
                                     "Target", G_TYPE_STRING, target.toUtf8().constData(),
                                     NULL);
}
