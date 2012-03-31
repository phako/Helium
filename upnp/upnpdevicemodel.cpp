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

#include <libgupnp/gupnp.h>
#include <libgssdp/gssdp.h>

#include <QDebug>

#include "refptrg.h"

#include "upnpdevicemodel.h"
#include "upnprenderer.h"
#include "upnpmediaserver.h"

UPnPDeviceModel *UPnPDeviceModel::instance;

void
UPnPDeviceModel::on_device_proxy_available(GUPnPControlPoint *cp,
                                           GUPnPDeviceProxy  *proxy,
                                           gpointer           user_data)
{
    Q_UNUSED(cp)

    UPnPDeviceModel *model = reinterpret_cast<UPnPDeviceModel*>(user_data);
    if (GUPNP_IS_DEVICE_INFO(proxy) == FALSE) {
        // should not happen
        return;
    }

    QMetaObject::invokeMethod(model, "onDeviceAvailable", Qt::QueuedConnection, Q_ARG(void *, g_object_ref(proxy)));
}

void UPnPDeviceModel::onDeviceAvailable(void *ptr)
{
    GUPnPDeviceInfo *device_info = GUPNP_DEVICE_INFO(ptr);

    QString udn = QString::fromUtf8(gupnp_device_info_get_udn(device_info));
    if (!m_deviceInfo.contains(udn)) {
        beginInsertRows(QModelIndex(), m_devices.count(), m_devices.count());
        m_deviceInfo.insert(udn, GUPNP_DEVICE_PROXY(ptr));
        m_devices << udn;
        endInsertRows();
    }
}

void
UPnPDeviceModel::on_device_proxy_unavailable(GUPnPControlPoint */*cp*/,
                                             GUPnPDeviceProxy  *proxy,
                                             gpointer           user_data)
{
    QString udn = QString::fromUtf8(gupnp_device_info_get_udn(GUPNP_DEVICE_INFO(proxy)));
    UPnPDeviceModel *model = reinterpret_cast<UPnPDeviceModel*>(user_data);

    QMetaObject::invokeMethod(model, "onDeviceUnavailable", Qt::QueuedConnection, Q_ARG(QString, udn));
}

void UPnPDeviceModel::onDeviceUnavailable(QString udn)
{
    int index = m_devices.indexOf(udn);
    if (index < 0) {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_devices.removeAt(index);
    g_object_unref(m_deviceInfo[udn]);
    m_deviceInfo.remove(udn);
    endRemoveRows();
    Q_EMIT deviceUnavailable(udn);
}

void
UPnPDeviceModel::on_context_available(GUPnPContextManager */*manager*/,
                                      GUPnPContext        *context,
                                      gpointer             user_data)
{
    UPnPDeviceModel *model = reinterpret_cast<UPnPDeviceModel*>(user_data);
    if (strcmp(gssdp_client_get_interface(GSSDP_CLIENT(context)), "lo") == 0) {
        qDebug () << "Ignoring localhost context to not show N9 server twice";

        return;
    }

    GUPnPControlPoint *cp = gupnp_control_point_new(context, GSSDP_ALL_RESOURCES);

    model->m_control_points << cp;

    g_signal_connect(cp,
                     "device-proxy-available",
                     G_CALLBACK(UPnPDeviceModel::on_device_proxy_available),
                     user_data);

    g_signal_connect(cp,
                     "device-proxy-unavailable",
                     G_CALLBACK(UPnPDeviceModel::on_device_proxy_unavailable),
                     user_data);

    gssdp_resource_browser_set_active(GSSDP_RESOURCE_BROWSER(cp), TRUE);

    qDebug() << "New context:" << gupnp_context_get_host_ip(context);
}

void
UPnPDeviceModel::on_context_unavailable(GUPnPContextManager */*manager*/,
                                        GUPnPContext        *context,
                                        gpointer             user_data)
{
    UPnPDeviceModel *model = reinterpret_cast<UPnPDeviceModel*>(user_data);

    QList<GUPnPControlPoint*>::iterator it = model->m_control_points.begin();
    while (it != model->m_control_points.end()) {
        if (gupnp_control_point_get_context(*it) == context) {
            g_object_unref(*it);
            it = model->m_control_points.erase(it);
        } else {
            ++it;
        }
    }
}

UPnPDeviceModel *UPnPDeviceModel::getDefault()
{
    if (UPnPDeviceModel::instance == 0) {
        UPnPDeviceModel::instance = new UPnPDeviceModel();
    }

    return UPnPDeviceModel::instance;
}

GUPnPDeviceProxy *UPnPDeviceModel::lookup(const QString &udn)
{
    UPnPDeviceModel *model = UPnPDeviceModel::getDefault();

    return model->m_deviceInfo[udn];
}

UPnPDeviceModel::UPnPDeviceModel(QObject *parent)
  : QAbstractListModel(parent)
  , m_ctx_manager(gupnp_context_manager_new (0, 0))
  , m_control_points()
  , m_devices()
{
    QHash<int, QByteArray> roles;

    roles[DeviceRoleFriendlyName] = "friendlyName";
    roles[DeviceRoleIcon] = "icon";
    roles[DeviceRoleUdn] = "udn";
    roles[DeviceRoleType] = "type";
    setRoleNames(roles);

    g_signal_connect (m_ctx_manager,
                      "context-available",
                      G_CALLBACK(on_context_available),
                      this);

    g_signal_connect (m_ctx_manager,
                      "context-unavailable",
                      G_CALLBACK(on_context_unavailable),
                      this);
}

UPnPDeviceModel::~UPnPDeviceModel()
{
    g_object_unref (m_ctx_manager);
    Q_FOREACH(GUPnPDeviceProxy *p, m_deviceInfo.values()) {
        g_object_unref(p);
    }
}

int UPnPDeviceModel::rowCount(const QModelIndex& /*parent*/) const
{
    return m_devices.count();
}

QVariant UPnPDeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    QString udn = m_devices.at(index.row());
    if (udn.isEmpty()) {
        return QVariant();
    }

    DeviceProxy proxy = DeviceProxy(m_deviceInfo[udn]);
    GUPnPDeviceInfo *info = GUPNP_DEVICE_INFO(proxy);

    if (info == 0) {
        return QVariant ();
    }

    switch (role) {
        case Qt::DisplayRole: // for use in QListView
        case DeviceRoleFriendlyName:
            return QString::fromUtf8(gupnp_device_info_get_friendly_name(info));
        case DeviceRoleIcon:
            return UPnPDevice::getIcon(proxy);
        case DeviceRoleUdn:
            return QString::fromUtf8(gupnp_device_info_get_udn(info));
        case DeviceRoleType:
            return QString::fromUtf8(gupnp_device_info_get_device_type(info));
        default:
            return QVariant();
    }

    return QVariant();
}

void UPnPDeviceModel::refresh()
{
    Q_FOREACH(GUPnPControlPoint *cp, m_control_points) {
        gssdp_resource_browser_set_active(GSSDP_RESOURCE_BROWSER(cp), FALSE);
    }

    Q_FOREACH(GUPnPControlPoint *cp, m_control_points) {
        gssdp_resource_browser_set_active(GSSDP_RESOURCE_BROWSER(cp), TRUE);
    }
}
