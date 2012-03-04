/*
This file is part of MediaController.

MediaController is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

MediaController is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with MediaController.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QListView>
#include <QLabel>

#include "upnpdevicemodel.h"
#include "upnpservermodel.h"
#include "upnprenderermodel.h"

Main::Main()
    : QMainWindow()
{
    QWidget *main = new QWidget (this);
    setCentralWidget(main);
    QVBoxLayout *layout = new QVBoxLayout;
    QListView *all = new QListView;
    all->setModelColumn(UPnPDeviceModel::DeviceRoleFriendlyName);
    all->setModel(UPnPDeviceModel::getDefault());
    QListView *server = new QListView;
    server->setModel(new UPnPServerModel ());
    server->setModelColumn(UPnPDeviceModel::DeviceRoleFriendlyName);
    connect (server, SIGNAL(activated(QModelIndex)), SLOT(onRowActivated(QModelIndex)));
    QListView *renderer = new QListView;
    renderer->setModel(new UPnPRendererModel ());
    renderer->setModelColumn(UPnPDeviceModel::DeviceRoleFriendlyName);
    m_browser = new QListView;

    layout->addWidget(new QLabel(QString::fromLatin1("All devices"), main));
    layout->addWidget(all);
    layout->addWidget(new QLabel(QString::fromLatin1("UPnP servers"), main));
    layout->addWidget(server);
    layout->addWidget(new QLabel(QString::fromLatin1("UPnP renderer"), main));
    layout->addWidget(renderer);
    layout->addWidget(new QLabel(QLatin1String("Browse test"), main));
    layout->addWidget(m_browser);
    main->setLayout(layout);
}

void
Main::onRowActivated(const QModelIndex &index)
{
    QString udn = index.data(UPnPDeviceModel::DeviceRoleUdn).toString();
    server.wrapDevice(udn);
    QAbstractListModel *model = server.browse();
    m_browser->setModel(model);
}

int main (int argc, char *argv[])
{
    QApplication app (argc, argv);
    Main m;

    m.show ();

    app.exec ();
}
