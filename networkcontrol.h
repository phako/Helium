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

#ifndef NETWORKCONTROL_H
#define NETWORKCONTROL_H

#include <QObject>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QtNetwork/QNetworkSession>

class NetworkControl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool online READ online NOTIFY onlineChanged)

public:
    explicit NetworkControl(QObject *parent = 0);

    Q_INVOKABLE void connect();
    bool online() { return m_manager.isOnline(); }

Q_SIGNALS:
    void onlineChanged(bool online);

private:
    QNetworkConfigurationManager m_manager;
    QNetworkSession              m_session;
};

#endif // NETWORKCONTROL_H
