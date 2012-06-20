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

#include "glib-utils.h"
#include "upnprenderer.h"

const QString START_POSITION = QLatin1String("0:00:00");

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
const char UPnPRenderer::RENDERING_CONTROL_SERVICE[] = "urn:schemas-upnp-org:service:RenderingControl";

void UPnPRenderer::setState(const QString &state)
{
    qDebug () << "New state" << state;
    m_state = state;
    if (state == QLatin1String("PLAYING")) {
        m_progressTimer.start(1000);
    } else {
        m_progressTimer.stop();
    }

    if (state == QLatin1String("STOPPED")) {
        setProgress(0.0);
        setPosition(START_POSITION);
    }

    Q_EMIT stateChanged();
}

void UPnPRenderer::setDuration(const QString& trackDuration)
{
    if (trackDuration == m_duration) {
        return;
    }

    m_duration = trackDuration;
    m_durationInSeconds = parseDurationString(m_duration);

    Q_EMIT durationChanged();
}

void UPnPRenderer::setProgress(float progress)
{
    if (fabs(progress - m_progress) < 0.00001) {
        return;
    }

    m_progress = progress;

    Q_EMIT progressChanged();
}

void UPnPRenderer::setProtocolInfo(const QString &protocolInfo)
{
    if (m_protocolInfo == protocolInfo) {
        return;
    }

    m_protocolInfo = protocolInfo;

    Q_EMIT protocolInfoChanged();
}

void UPnPRenderer::setTitle(const QString& uri)
{
    if (uri == m_currentTitle) {
        return;
    }

    m_currentTitle = uri;

    Q_EMIT titleChanged();
}

void UPnPRenderer::setCanPause(bool canPause)
{
    if (canPause == this->m_canPause) {
        return;
    }

    m_canPause = canPause;

    Q_EMIT canPauseChanged();
}

void UPnPRenderer::setPosition(const QString &position)
{
    if (position == m_position) {
        return;
    }

    m_position = position;

    Q_EMIT positionChanged();
}

void UPnPRenderer::setCanSeek(bool canSeek)
{
    if (canSeek == m_canSeek) {
        return;
    }

    m_canSeek = canSeek;

    Q_EMIT canSeekChanged();
}

void UPnPRenderer::setSeekMode(const QString &seekMode)
{
    if (seekMode == m_seekMode) {
        return;
    }

    m_seekMode = seekMode;

    Q_EMIT seekModeChanged();
}

void UPnPRenderer::setMute(bool mute)
{
    if (m_mute == mute) {
        return;
    }

    m_mute = mute;

    Q_EMIT muteChanged();
}

void UPnPRenderer::setVolume(unsigned int volume)
{
    if (m_volume == volume) {
        return;
    }

    m_volume = volume;

    Q_EMIT volumeChanged();
}

void UPnPRenderer::setMaxVolume(unsigned int maxVolume)
{
    if (m_maxVolume == maxVolume) {
        return;
    }

    m_maxVolume = maxVolume;

    Q_EMIT maxVolumeChanged();
}

void UPnPRenderer::setCanMute(bool canMute)
{
    if (m_canMute == canMute) {
        return;
    }

    m_canMute = canMute;

    Q_EMIT canMuteChanged();
}

void UPnPRenderer::setCanVolume(bool canVolume)
{
    if (m_canVolume == canVolume) {
        return;
    }

    m_canVolume = canVolume;

    Q_EMIT canVolumeChanged();
}

static void
on_didl_item_available (GUPnPDIDLLiteParser *parser, GUPnPDIDLLiteItem *item, gpointer user_data)
{
    Q_UNUSED(parser)
    GUPnPDIDLLiteObject **object = static_cast<GUPnPDIDLLiteObject **>(user_data);

    if (*object != 0) {
        return;
    }

    *object = GUPNP_DIDL_LITE_OBJECT(g_object_ref(item));
}

UPnPRenderer::UPnPRenderer()
    : UPnPDevice()
    , m_lastChangeParser(RefPtrG<GUPnPLastChangeParser>::wrap(gupnp_last_change_parser_new()))
    , m_avTransport()
    , m_connectionManager()
    , m_renderingControl(0)
    , m_state(QLatin1String("NO_MEDIA_PRESENT"))
    , m_protocolInfo(QLatin1String("*:*:*:*"))
    , m_duration(START_POSITION)
    , m_progressTimer()
    , m_canPause(false)
    , m_currentTitle()
    , m_position(START_POSITION)
    , m_canSeek(false)
    , m_seekMode(QLatin1String(""))
{
    connect(&m_progressTimer, SIGNAL(timeout()), SLOT(onProgressTimeout()));
}

void UPnPRenderer::onLastChange(const QString &name, const QVariant &value)
{
    Q_UNUSED(name);

    qDebug() << value.toString();
    GError *error = 0;

    if (sender() == m_renderingControl.data()) {
        char *mute_str = 0, *volume_str = 0;
        gboolean mute;
        unsigned int volume;

        if (gupnp_last_change_parser_parse_last_change(m_lastChangeParser,
                                                       0,
                                                       value.toString().toUtf8().constData(),
                                                       &error,
                                                       "Mute", G_TYPE_STRING, &mute_str,
                                                       "Mute", G_TYPE_BOOLEAN, &mute,
                                                       "Volume", G_TYPE_STRING, &volume_str,
                                                       "Volume", G_TYPE_UINT, &volume,
                                                       NULL)) {
            if (mute_str != 0) {
                setMute(mute == TRUE);
                g_free(mute_str);
            }

            if (volume_str != 0) {
                setVolume(volume);
                g_free(volume_str);
            }
        } else {
            qDebug() << "Failed to parse last change" << error->message;
            g_error_free(error);
        }
    } else if (sender() == m_avTransport.data()){
        char *state_name = 0;
        char *track_duration = 0;
        char *track_uri = 0;
        char *track_meta_data = 0;
        char *av_transport_meta_data = 0;
        const char *meta_data = 0;

        if (gupnp_last_change_parser_parse_last_change(m_lastChangeParser,
                                                       0,
                                                       value.toString().toUtf8().constData(),
                                                       &error,
                                                       "TransportState", G_TYPE_STRING, &state_name,
                                                       "CurrentTrackDuration", G_TYPE_STRING, &track_duration,
                                                       "CurrentTrackURI", G_TYPE_STRING, &track_uri,
                                                       "AVTransportURIMetaData", G_TYPE_STRING, &av_transport_meta_data,
                                                       "CurrentTrackMetadata", G_TYPE_STRING, &track_meta_data,
                                                       NULL)) {
            if (state_name != 0) {
                setState(QString::fromUtf8(state_name));

                g_free(state_name);
            }

            if (track_duration != 0) {
                setDuration(QString::fromUtf8(track_duration));

                g_free(track_duration);
            }

            if (track_meta_data != 0) {
                meta_data = track_meta_data;
            } else if (av_transport_meta_data != 0) {
                meta_data = av_transport_meta_data;
            }

            if (meta_data != 0) {
                DIDLLiteParser parser = DIDLLiteParser::wrap(gupnp_didl_lite_parser_new ());
                GError *error = 0;
                GUPnPDIDLLiteObject *object = 0;

                g_signal_connect (G_OBJECT(parser), "item-available", G_CALLBACK (on_didl_item_available), &object);

                if (gupnp_didl_lite_parser_parse_didl(parser, meta_data, &error)) {
                    if (object != 0) {
                        setTitle(QString::fromUtf8(gupnp_didl_lite_object_get_title(object)));
                    }
                }

                if (object != 0) {
                    g_object_unref(object);
                }
            }

            if (track_meta_data != 0) {
                g_free(track_meta_data);
            }

            if (av_transport_meta_data != 0) {
                g_free(av_transport_meta_data);
            }

            if (m_currentTitle.isEmpty() && track_uri != 0) {
                setTitle(QString::fromUtf8(track_uri));

                g_free(track_uri);
            }
        } else {
            qDebug() << "Failed to parse last change" << error->message;
            g_error_free(error);
        }
    }
}

void UPnPRenderer::onProgressTimeout()
{
    queueCall(m_avTransport->call(QLatin1String("GetPositionInfo"),
                                  QLatin1String("InstanceID"), 0),
              SLOT(onGetPositionInfoReady()));
}

void UPnPRenderer::unsubscribe()
{
    static const QString LAST_CHANGE = QLatin1String("LastChange");

    if (not m_renderingControl.isNull() && m_renderingControl->subscribed()) {
        m_renderingControl->setSubscribed(false);
        m_renderingControl->removeNotify(LAST_CHANGE);
        m_renderingControl->disconnect(this, SLOT(onLastChange(QString,QVariant)));
    }

    if (not m_avTransport.isNull()) {
        m_avTransport->setSubscribed(false);
        m_avTransport->removeNotify(LAST_CHANGE);
        m_avTransport->disconnect(this, SLOT(onLastChange(QString,QVariant)));
    }
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
    Q_EMIT availableChanged();

    // reset to initial state
    setState(QLatin1String("NO_MEDIA_PRESENT"));
    setProtocolInfo(QLatin1String("*:*:*:*"));
    setDuration(START_POSITION);
    setCanPause(false);
    setTitle(QString());
    setPosition(START_POSITION);
    setProgress(0.0f);
    m_avTransport.reset(0);
    m_connectionManager.reset(0);
    m_renderingControl.reset(0);

    if (m_proxy.isEmpty()) {
        return;
    }


    m_avTransport.reset(getService(UPnPRenderer::AV_TRANSPORT_SERVICE));
    m_connectionManager.reset(getService(UPnPDevice::CONNECTION_MANAGER_SERVICE));
    m_renderingControl.reset(getService(UPnPRenderer::RENDERING_CONTROL_SERVICE));

    m_avTransport->addNotify(QLatin1String("LastChange"));
    m_avTransport->setSubscribed(true);
    connect(m_avTransport.data(), SIGNAL(notify(QString,QVariant)), SLOT(onLastChange(QString,QVariant)));

    queueCall(m_connectionManager->call(QLatin1String("GetProtocolInfo")),
              SLOT(onGetProtocolInfo()));
}


void UPnPRenderer::onGetPositionInfoReady()
{
    ServiceProxyCall *call = qobject_cast<ServiceProxyCall *>(sender());

    if (call == 0) {
        return;
    }

    call->finalize(QStringList() << QLatin1String("RelTime"));
    call->deleteLater();

    if (call->hasError()) {
        Q_EMIT error(call->errorCode(), call->errorMessage());

        return;
    }

    QString relTime = call->get(QLatin1String("RelTime")).toString();
    setPosition(relTime);
    setProgress((double) parseDurationString(relTime) / (double) m_durationInSeconds);
}

void UPnPRenderer::onGetProtocolInfo()
{
    auto call = qobject_cast<ServiceProxyCall *>(sender());

    if (call == 0) {
        return;
    }

    call->finalize(QStringList() << QLatin1String("Sink"));
    call->deleteLater();

    if (call->hasError()) {
        Q_EMIT error(call->errorCode(), call->errorMessage());

        return;
    }

    if (call->get(QLatin1String("Sink")).isValid()) {
        setProtocolInfo(call->get(QLatin1String("Sink")).toString());
    }

    connect(m_avTransport.data(), SIGNAL(introspectionReady()), SLOT(onAVTransportIntrospectionReady()));
    m_avTransport->introspect();
}

void UPnPRenderer::onAVTransportIntrospectionReady()
{
    auto introspection = m_avTransport->introspection();
    // TODO: Handle error
    setCanPause(introspection->hasAction(QLatin1String("Pause")));
    auto seekModeInfo = introspection->variable(QLatin1String("A_ARG_TYPE_SeekMode"));
    Q_FOREACH(QString seekMode, seekModeInfo.allowedValues()) {
        if (seekMode == QLatin1String("ABS_TIME") ||
            seekMode == QLatin1String("REL_TIME")) {
            setCanSeek(true);
            setSeekMode(seekMode);

            break;
        }
    }

    connect(m_renderingControl.data(), SIGNAL(introspectionReady()), SLOT(onRenderingControlIntrospectionReady()));
    m_renderingControl->introspect();
}

void UPnPRenderer::onRenderingControlIntrospectionReady()
{
    ServiceIntrospection *introspection = m_renderingControl->introspection();

    qDebug() << "Got introspection!";
    setCanMute(introspection->hasAction(QLatin1String("SetMute")));
    setCanVolume(introspection->hasAction(QLatin1String("SetVolume")));
    if (canVolume()) {
        auto maxVolume = introspection->variable(QLatin1String("Volume")).maximum();
        if (not maxVolume.isNull()) {
            setMaxVolume(maxVolume.toUInt());
        }
    }

    if (canMute() || canVolume()) {
        m_renderingControl->addNotify(QLatin1String("LastChange"));
        connect(m_renderingControl.data(), SIGNAL(notify(QString, QVariant)), SLOT(onLastChange(QString,QVariant)));
        m_renderingControl->setSubscribed(true);
    }

    Q_EMIT ready();
}

void UPnPRenderer::onSetAVTransportUri()
{
    auto f = qobject_cast<ServiceProxyCall *>(sender());
    QScopedPointer<ServiceProxyCall, ScopedPointerLater<ServiceProxyCall> > call(f);

    if (call.isNull()) {
        return;
    }

    unqueueCall(call.data(), QStringList(), false);

    if (call->hasError()) {
        if (call->errorCode() == 705) {
            // Transport locked. Call Stop and try again
            // Do not delete call as we chained up
            stop(call.take());
        } else {
            Q_EMIT error(call->errorCode(), call->errorMessage());
        }

        return;
    }

    if (call->next() != 0) {
        // prevent call's destructor from clearing next
        auto next = call->next();
        call->setNext(0);
        queueCall(next);
    }
}


void UPnPRenderer::setAVTransportUri(const QString &uri, const QString &metaData)
{
    setAVTransportUri(uri, metaData, 0);
}

void UPnPRenderer::setAVTransportUri(const QString &uri, const QString &metaData, ServiceProxyCall *next)
{
    if (m_avTransport.isNull()) {
        return;
    }

    auto call = m_avTransport->call(QLatin1String("SetAVTransportURI"),
                                    QLatin1String("InstanceID"), QLatin1String("0"),
                                    QLatin1String("CurrentURI"), uri,
                                    QLatin1String("CurrentURIMetaData"), metaData);
    call->setNext(next);
    queueCall(call, SLOT(onSetAVTransportUri()));
}

void UPnPRenderer::setUriAndPlay(const QString& uri, const QString& metaData)
{
    if (m_avTransport.isNull()) {
        return;
    }

    auto playCall = m_avTransport->call(QLatin1String("Play"),
                                        QLatin1String("InstanceID"), QLatin1String("0"),
                                        QLatin1String("Speed"), QLatin1String("1"));
    setAVTransportUri(uri, metaData, playCall);
}

void UPnPRenderer::onPause ()
{
    ServiceProxyCall *call = qobject_cast<ServiceProxyCall *>(sender());

    if (call == 0) {
        return;
    }

    unqueueCall(call);

    if (not call->hasError()) {
        return;
    }

    if (call->errorCode() == 602 || call->errorCode() == 401) {
        qDebug() << "Device does not implement pause";
        setCanPause(false);
    }
}

void UPnPRenderer::play()
{
    if (m_avTransport.isNull() || m_state == QLatin1String("PLAYING")) {
        return;
    }

    queueCall(m_avTransport->call(QLatin1String("Play"),
                                  QLatin1String("InstanceID"), QLatin1String("0"),
                                  QLatin1String("Speed"), QLatin1String("1")));
}

void UPnPRenderer::stop()
{
    stop(0);
}

void UPnPRenderer::stop(ServiceProxyCall *next)
{
    if (m_avTransport.isNull() || m_state == QLatin1String("STOPPED")) {
        return;
    }

    auto call = m_avTransport->call(QLatin1String("Stop"),
                                    QLatin1String("InstanceID"), QLatin1String("0"));
    call->setNext(next);
    queueCall(call);
}

void UPnPRenderer::pause()
{
    if (m_avTransport.isNull() || m_state == QLatin1String("PAUSED_PLAYBACK")) {
        return;
    }

    queueCall(m_avTransport->call(QLatin1String("Pause"),
                                  QLatin1String("InstanceID"), QLatin1String("0")),
              SLOT(onPause()));
}

void UPnPRenderer::seekRelative(float percent)
{
    if (not canSeek()) {
        return;
    }
    
    QString target = getRelativeTime(percent);

    queueCall(m_avTransport->call(QLatin1String("Seek"),
                                  QLatin1String("InstanceID"), QLatin1String("0"),
                                  QLatin1String("Unit"), m_seekMode,
                                  QLatin1String("Target"), target));
}

QString UPnPRenderer::getRelativeTime(float percent)
{
    if (percent < 0.0) {
        percent = 0.0;
    } else if (percent > 1.0) {
        percent = 1.0;
    }

    quint64 position = percent * m_durationInSeconds;

    int hours = position / 3600;
    position %= 3600;
    int minutes = position / 60;
    int seconds = position % 60;

    return QString::fromLatin1("%1:%2:%3").arg(hours)
                                          .arg(minutes, 2, 10, QLatin1Char('0'))
                                          .arg(seconds, 2, 10, QLatin1Char('0'));
}

void UPnPRenderer::setRemoteMute(bool mute)
{
    if (not canMute() || m_renderingControl.isNull() || mute == m_mute) {
        return;
    }

    m_mute = mute;
    queueCall(m_renderingControl->call(QLatin1String("SetMute"),
                                       QLatin1String("InstanceID"), QLatin1String("0"),
                                       QLatin1String("Channel"), QLatin1String("Master"),
                                       QLatin1String("DesiredMute"), mute));
}

void UPnPRenderer::setRemoteVolume(unsigned int volume)
{
    if (not canVolume() || m_renderingControl.isNull() || volume == m_volume) {
        return;
    }

    m_volume = volume;
    queueCall(m_renderingControl->call(QLatin1String("SetVolume"),
                                       QLatin1String("InstanceID"), QLatin1String("0"),
                                       QLatin1String("Channel"), QLatin1String("Master"),
                                       QLatin1String("DesiredVolume"), volume));
}
