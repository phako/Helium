/*
This file is part of Helium.

Copyright (C) 2011-2012 Jens Georg <mail+helium@jensge.org>

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

#ifndef UPNPRENDERER_H
#define UPNPRENDERER_H

#include <QObject>
#include <QTimer>
#include <QtCore/QList>

#include "gupnp-av/gupnp-av.h"

#include "serviceproxy.h"
#include "upnpdevice.h"

class UPnPRenderer : public UPnPDevice
{
    Q_OBJECT
    Q_PROPERTY(QString state READ state NOTIFY stateChanged REVISION 1)
    Q_PROPERTY(QString duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QString protocolInfo READ protocolInfo NOTIFY protocolInfoChanged)
    Q_PROPERTY(float progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool canPause READ canPause NOTIFY canPauseChanged)
    Q_PROPERTY(bool canSeek READ canSeek NOTIFY canSeekChanged)
    Q_PROPERTY(QString seekMode READ seekMode NOTIFY seekModeChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString position READ position NOTIFY positionChanged)
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)
    Q_PROPERTY(bool canMute READ canMute NOTIFY canMuteChanged)
    Q_PROPERTY(bool canVolume READ canVolume NOTIFY canVolumeChanged)
    Q_PROPERTY(bool mute READ mute WRITE setRemoteMute NOTIFY muteChanged)
    Q_PROPERTY(unsigned int volume READ volume WRITE setRemoteVolume NOTIFY volumeChanged)
    Q_PROPERTY(unsigned int maxVolume READ maxVolume NOTIFY maxVolumeChanged)
public:
    static const char DEVICE_TYPE[];
    static const char AV_TRANSPORT_SERVICE[];
    static const char RENDERING_CONTROL_SERVICE[];

    explicit UPnPRenderer();
    ~UPnPRenderer();
    
    // Property getters
    QString state() { return m_state; }
    QString duration() { return m_duration; }
    QString protocolInfo() { return m_protocolInfo; }
    float progress() { return m_progress; }
    QString title() { return m_currentTitle; }
    QString position() { return m_position; }
    bool canPause() const { return m_canPause; }
    bool canSeek() const { return m_canSeek; }
    QString seekMode() { return m_seekMode; }
    bool available() { return not m_proxy.isEmpty(); }
    bool canMute() const { return m_canMute; }
    bool mute() const { return m_mute; }
    void setRemoteMute(bool mute);

    bool canVolume() const { return m_canVolume; }
    unsigned int volume() const { return m_volume; }
    unsigned int maxVolume() const { return m_maxVolume; }
    void setRemoteVolume(unsigned int volume);

    // QML invokable functions
    Q_INVOKABLE virtual void wrapDevice(const QString &udn);

    // UPnP RPC calls
    // AVTransport:1 mandatory
    Q_INVOKABLE void setAVTransportUri(const QString& uri, const QString& metaData = QLatin1String(""));
    Q_INVOKABLE void setUriAndPlay(const QString& uri, const QString& metaData = QLatin1String(""));
    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void seekRelative(float percent);
    Q_INVOKABLE QString getRelativeTime(float percent);

    // AVTransport:1 optional
    Q_INVOKABLE void pause();

Q_SIGNALS:

    // Property notifiers
    void stateChanged(void);
    void protocolInfoChanged(void);
    void durationChanged(void);
    void progressChanged(void);
    void canPauseChanged(void);
    void titleChanged(void);
    void positionChanged(void);
    void canSeekChanged(void);
    void seekModeChanged(void);
    void availableChanged(void);
    void canMuteChanged(void);
    void muteChanged(void);
    void canVolumeChanged(void);
    void volumeChanged(void);
    void maxVolumeChanged(void);

private Q_SLOTS:
    void onProgressTimeout();
    void onLastChange(const QString &name, const QVariant &value);
    void onServiceProxyCallReady();
    void onRenderingControlIntrospectionReady();
    void onAVTransportIntrospectionReady();
    void onGetPositionInfoReady();
    void onPause();
    void onSetAVTransportUri();

private:
    // private property setters
    void setState(const QString &state);
    void setDuration(const QString &duration);
    void setProgress(float progress);
    void setProtocolInfo(const QString &protocolInfo);
    void setTitle(const QString &uri);
    void setCanPause(bool canPause);
    void setPosition(const QString &position);
    void setCanSeek(bool canSeek);
    void setSeekMode(const QString &seekMode);
    void setCanMute(bool canMute);
    void setCanVolume(bool canVolume);
    void setMaxVolume(unsigned int maxVolume);
    void setMute(bool mute);
    void setVolume(unsigned int volume);

    void stop(ServiceProxyCall *next);
    void setAVTransportUri(const QString &uri, const QString &metaData, ServiceProxyCall *next);


    static void on_get_protocol_info(GUPnPServiceProxy       *proxy,
                                     GUPnPServiceProxyAction *action,
                                     gpointer                 user_data);

    void unsubscribe();
    void handleLastCall(const char *slot = SLOT(onServiceProxyCallReady()));

    RefPtrG<GUPnPLastChangeParser> m_lastChangeParser;
    QScopedPointer<ServiceProxy> m_avTransport;
    GServiceProxy m_connectionManager;
    QScopedPointer<ServiceProxy> m_renderingControl;
    QString m_state;
    QString m_protocolInfo;
    QString m_duration;
    QTimer m_progressTimer;
    quint64 m_durationInSeconds;
    quint64 m_currentPosition;
    float m_progress;
    bool m_canPause;
    QString m_currentTitle;
    QString m_position;
    bool m_canSeek;
    QString m_seekMode;
    bool m_canVolume;
    unsigned int m_volume;
    unsigned int m_maxVolume;
    bool m_canMute;
    bool m_mute;
    QList<ServiceProxyCall *> m_pendingCalls;
};

#endif // UPNPRENDERER_H
