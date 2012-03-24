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

#ifndef UPNPRENDERER_H
#define UPNPRENDERER_H

#include <QObject>
#include <QTimer>

#include <libgupnp-av/gupnp-av.h>

#include "upnpdevice.h"

class UPnPRenderer : public UPnPDevice
{
    Q_OBJECT
    Q_PROPERTY(QString state READ state NOTIFY stateChanged REVISION 1)
    Q_PROPERTY(QString duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QString protocolInfo READ protocolInfo NOTIFY protocolInfoChanged)
    Q_PROPERTY(float progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool canPause READ canPause NOTIFY canPauseChanged)
    Q_PROPERTY(QString uri READ uri NOTIFY uriChanged)
public:
    static const char DEVICE_TYPE[];
    static const char AV_TRANSPORT_SERVICE[];

    explicit UPnPRenderer();
    ~UPnPRenderer();
    
    QString state() { return m_state; }
    void setState(const QString& state);

    QString duration() { return m_duration; }
    void setDuration(const QString& duration);

    QString protocolInfo() { return m_protocolInfo; }

    float progress() { return m_progress; }

    QString uri() { return m_uri; }
    void setURI(const QString &uri);

    Q_INVOKABLE virtual void wrapDevice(const QString &udn);

    Q_INVOKABLE bool canPause() const { return m_canPause; }
Q_SIGNALS:
    void stateChanged(void);
    void protocolInfoChanged(void);
    void durationChanged(void);
    void progressChanged(void);
    void canPauseChanged(void);
    void uriChanged(void);

public Q_SLOTS:
    // AVTransport:1 mandatory
    void setAVTransportUri(const QString& uri, const QString& metaData = QLatin1String(""));
    void play();
    void stop();
    void seekRelative(float percent);

    // AVTransport:1 optional
    void pause();
private Q_SLOTS:
    void onProgressTimeout();
private:
    static void on_get_position_info(GUPnPServiceProxy       *proxy,
                                     GUPnPServiceProxyAction *action,
                                     gpointer                 user_data);
    static void on_set_av_transport_uri(GUPnPServiceProxy       *proxy,
                                        GUPnPServiceProxyAction *action,
                                        gpointer                 user_data);

    static void on_play(GUPnPServiceProxy       *proxy,
                        GUPnPServiceProxyAction *action,
                        gpointer                 user_data);

    static void on_get_protocol_info(GUPnPServiceProxy       *proxy,
                                     GUPnPServiceProxyAction *action,
                                     gpointer                 user_data);

    static void on_transport_state_changed(GUPnPServiceProxy */*service*/,
                                           const char        */*variable*/,
                                           GValue            *value,
                                           gpointer           user_data);

    static void on_got_introspection (GUPnPServiceInfo *info,
                                      GUPnPServiceIntrospection *introspection,
                                      const GError *error,
                                      gpointer user_data);

    void on_last_change(const char *last_change);
    void unsubscribe();

    RefPtrG<GUPnPLastChangeParser> m_lastChangeParser;
    ServiceProxy m_avTransport;
    ServiceProxy m_connectionManager;
    QString m_state;
    QString m_protocolInfo;
    QString m_duration;
    QTimer m_progressTimer;
    quint64 m_durationInSeconds;
    quint64 m_currentPosition;
    float m_progress;
    bool m_canPause;
    QString m_uri;
};

#endif // UPNPRENDERER_H
