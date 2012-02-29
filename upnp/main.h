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

#ifndef __MAIN_H__
#define __MAIN_H__

#include <QMainWindow>
#include <QListView>
#include "upnpmediaserver.h"

class Main : public QMainWindow {
    Q_OBJECT
public:
    explicit Main();
    QListView *m_browser;
    UPnPMediaServer server;
private Q_SLOTS:
    void onRowActivated(const QModelIndex& index);
};
#endif // __MAIN_H__
