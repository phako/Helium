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

#ifndef UPNPSERVERMODEL_H
#define UPNPSERVERMODEL_H

#include <QSortFilterProxyModel>
#include "upnpmediaserver.h"

class UPnPServerModel : public QSortFilterProxyModel 
{
    Q_OBJECT
public:
    explicit UPnPServerModel(QObject *parent = 0);
    
Q_SIGNALS:
    
public Q_SLOTS:
    QString get(int row) const;
};

#endif // UPNPSERVERMODEL_H
