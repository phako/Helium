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

#include "upnprenderermodel.h"
#include "upnpdevicemodel.h"

UPnPRendererModel::UPnPRendererModel(QObject *parent)
   : QSortFilterProxyModel (parent)
{
    setSourceModel(UPnPDeviceModel::getDefault());
    setFilterRole(UPnPDeviceModel::DeviceRoleType);
    setFilterRegExp(QRegExp("^urn:schemas-upnp-org:device:MediaRenderer:\\d+"));
}

QString UPnPRendererModel::get(int row) const
{
    QModelIndex index = mapToSource(this->index(row, 0));
    return sourceModel()->data(index, UPnPDeviceModel::DeviceRoleUdn).toString();
}

void UPnPRendererModel::refresh() const
{
    UPnPDeviceModel::getDefault()->refresh();
}
