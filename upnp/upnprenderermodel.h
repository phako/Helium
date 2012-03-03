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

#ifndef UPNPRENDERERMODEL_H
#define UPNPRENDERERMODEL_H

#include <QSortFilterProxyModel>

class UPnPRendererModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    UPnPRendererModel(QObject *parent = 0);
public Q_SLOTS:
    void refresh() const;
    QString get(int index) const;
};

#endif // UPNPRENDERERMODEL_H
