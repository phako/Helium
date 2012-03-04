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

#ifndef UPNPDEVICELISTER_H
#define UPNPDEVICELISTER_H

#include <libgupnp/gupnp.h>

#include <QObject>
#include <QAbstractListModel>

#include "upnpdevice.h"

class UPnPDeviceModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum DeviceRoles {
        DeviceRoleFriendlyName = Qt::UserRole + 1,
        DeviceRoleIcon,
        DeviceRoleUdn,
        DeviceRoleType
    };

    UPnPDeviceModel(QObject *parent = 0);
    ~UPnPDeviceModel();

    // virtual functions from QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;    

    static UPnPDeviceModel *getDefault();
    static GUPnPDeviceProxy *lookup(const QString& udn);

    Q_INVOKABLE void refresh();
private Q_SLOTS:
    void onDeviceUnavailable(QString udn);
    void onDeviceAvailable(void *device_info);

    void onRowsInserted() {
        qDebug() << "Rows inserted";
    }

private:
    static UPnPDeviceModel *instance;

    // GUPnP callbacks
    static void on_device_proxy_available(GUPnPControlPoint *cp,
                                          GUPnPDeviceProxy  *proxy,
                                          gpointer           user_data);
    static void on_device_proxy_unavailable(GUPnPControlPoint *cp,
                                            GUPnPDeviceProxy  *proxy,
                                            gpointer           user_data);
    static void on_context_available(GUPnPContextManager *,
                                     GUPnPContext        *context,
                                     gpointer             user_data);
    static void on_context_unavailable(GUPnPContextManager *manager,
                                       GUPnPContext        *context,
                                       gpointer             user_data);
private:
    GUPnPContextManager               *m_ctx_manager;
    QList<GUPnPControlPoint *>         m_control_points;
    QList<QString>                     m_devices;
    QHash<QString, GUPnPDeviceProxy *> m_deviceInfo;
};

#endif // UPNPDEVICELISTER_H
