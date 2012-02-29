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
    GUPnPDeviceInfo *device_info = GUPNP_DEVICE_INFO (proxy);
    if (device_info == 0) {
        // should not happen
        return;
    }

    QString udn = QString::fromUtf8(gupnp_device_info_get_udn(device_info));
    if (!model->m_deviceInfo.contains(udn)) {
        model->beginInsertRows(QModelIndex(),
                               model->m_devices.count(),
                               model->m_devices.count());
        model->m_deviceInfo.insert(udn,
                                   GUPNP_DEVICE_PROXY(g_object_ref(proxy)));
        model->m_devices << udn;
        model->endInsertRows();
    }
}

void
UPnPDeviceModel::on_device_proxy_unavailable(GUPnPControlPoint */*cp*/,
                                             GUPnPDeviceProxy  *proxy,
                                             gpointer           user_data)
{
    QString udn = QString::fromUtf8(gupnp_device_info_get_udn(GUPNP_DEVICE_INFO(proxy)));
    UPnPDeviceModel *model = reinterpret_cast<UPnPDeviceModel*>(user_data);

    int index = model->m_devices.indexOf(udn);
    if (index < 0) {
        return;
    }

    model->beginRemoveRows(QModelIndex(), index, index);
    model->m_devices.removeAt(index);
    g_object_unref(model->m_deviceInfo[udn]);
    model->m_deviceInfo.remove(udn);
    model->endRemoveRows();
}

void
UPnPDeviceModel::on_context_available(GUPnPContextManager *manager,
                                      GUPnPContext        *context,
                                      gpointer             user_data)
{
    if (strcmp(gssdp_client_get_interface(GSSDP_CLIENT(context)), "wlan0") != 0) {
        qDebug () << "Ignoring localhost context to not show N9 server twice";

        return;
    }

    GUPnPControlPoint *cp = gupnp_control_point_new(context, GSSDP_ALL_RESOURCES);
    gupnp_context_manager_manage_control_point (manager, cp);

    // context manager takes ownership through extra ref
    g_object_unref (cp);

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
                                        gpointer             /*user_data*/)
{
    qDebug() << "Context unavailable:" << gupnp_context_get_host_ip(context);
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

bool UPnPDeviceModel::enabled()
{
    return true;
}

void UPnPDeviceModel::setEnabled(bool enabled)
{
    Q_UNUSED(enabled)
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
