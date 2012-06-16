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

#ifndef UPNPDEVICE_H
#define UPNPDEVICE_H

#include <QObject>
#include <QUrl>
#include <QtCore/QStringList>

#include "refptrg.h"

class ServiceProxy;
class ServiceProxyCall;
class UPnPDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString friendlyName READ friendlyName NOTIFY friendlyNameChanged REVISION 1)
    Q_PROPERTY(QString udn READ udn REVISION 1)
    Q_PROPERTY(QUrl icon READ icon NOTIFY iconChanged REVISION 1)
    Q_PROPERTY(QString type READ type REVISION 1)
public:
    explicit UPnPDevice();
    UPnPDevice(const UPnPDevice& other);
    ~UPnPDevice();

    QString friendlyName() const;
    QUrl icon() const;
    QString udn() const;
    QString type() const;
    Q_INVOKABLE virtual void wrapDevice(const QString& udn);
    ServiceProxy* getService(const char *service) const;

    // static helper functions
    static QUrl getIcon(GUPnPDeviceProxy *proxy);

    static const char CONNECTION_MANAGER_SERVICE[];
Q_SIGNALS:
    Q_REVISION(1) void friendlyNameChanged();
    Q_REVISION(1) void iconChanged();
    void unavailable();
    void error(int code, const QString& message);
    void ready();

private Q_SLOTS:
    void onDeviceUnavailable(const QString& udn);
    void defaultServiceProxyCallHandler();
private:
    QList<ServiceProxyCall *> m_pendingCalls;
protected:
    DeviceProxy m_proxy;

    void queueCall(ServiceProxyCall *call, const char *slot = SLOT(defaultServiceProxyCallHandler()));
    void unqueueCall(ServiceProxyCall *call, const QStringList &args = QStringList(), bool freeCall = true);
};

#endif // UPNPDEVICE_H
