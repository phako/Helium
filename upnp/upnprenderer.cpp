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

#include "upnprenderer.h"

const char UPnPRenderer::DEVICE_TYPE[] = "urn:schemas-upnp-org:device:MediaRenderer:";
const char UPnPRenderer::AV_TRANSPORT_SERVICE[] = "urn:schemas-upnp-org:service:AVTransport";

void UPnPRenderer::setState(const QString &state)
{
    qDebug () << "New state" << state;
    m_state = state;
    QMetaObject::invokeMethod(this, "stateChanged", Qt::QueuedConnection);
}

void
UPnPRenderer::on_transport_state_changed (GUPnPServiceProxy */*service*/,
                                          const char        */*variable*/,
                                          GValue            *value,
                                          gpointer           user_data)
{
    UPnPRenderer *renderer = reinterpret_cast<UPnPRenderer*>(user_data);
    GError *error = 0;
    char *state_name;

    if (gupnp_last_change_parser_parse_last_change(renderer->m_lastChangeParser,
                                                   0,
                                                   g_value_get_string(value),
                                                   &error,
                                                   "TransportState",
                                                   G_TYPE_STRING,
                                                   &state_name,
                                                   NULL)) {
        renderer->setState(QString::fromUtf8(state_name));

        g_free(state_name);
    } else {
        qDebug() << "Failed to parse last change" << error->message;
        g_error_free(error);
    }
}

UPnPRenderer::UPnPRenderer()
    : UPnPDevice()
    , m_lastChangeParser(RefPtrG<GUPnPLastChangeParser>::wrap(gupnp_last_change_parser_new()))
{
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

    if (m_proxy.isEmpty()) {
        return;
    }

    m_avTransport = getService(UPnPRenderer::AV_TRANSPORT_SERVICE);
    gupnp_service_proxy_add_notify(m_avTransport,
                                   "LastChange", G_TYPE_STRING,
                                   UPnPRenderer::on_transport_state_changed,
                                   this);

    gupnp_service_proxy_set_subscribed(m_avTransport, TRUE);
}

void UPnPRenderer::on_set_av_transport_uri (GUPnPServiceProxy       *proxy,
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
        qDebug() << "Failed to set the transport URI:" << error->message;
        g_error_free (error);
    }
}

void UPnPRenderer::setAVTransportUri(const QString &uri)
{
    gupnp_service_proxy_begin_action(m_avTransport,
                                     "SetAVTransportURI",
                                     UPnPRenderer::on_set_av_transport_uri,
                                     this,
                                     "InstanceID", G_TYPE_STRING, "0",
                                     "CurrentURI", G_TYPE_STRING, uri.toUtf8().constData(),
                                     "CurrentURIMetaData", G_TYPE_STRING, "",
                                     NULL);
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

