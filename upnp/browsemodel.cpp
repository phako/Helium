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

#include "browsemodel.h"
#include "browsemodel_p.h"

BrowseModel empty;

BrowseModel::BrowseModel(ServiceProxyCall *call, const QString &protocolInfo, QObject *parent)
    : QSortFilterProxyModel(parent)
    , d_ptr(new BrowseModelPrivate(call, protocolInfo, this))
{
    setSourceModel(d_ptr);
    setFilterRole(BrowseModelPrivate::BrowseRoleTitle);
    setFilterCaseSensitivity(Qt::CaseInsensitive);

    connect(d_ptr, SIGNAL(busyChanged()), SIGNAL(busyChanged()));
    connect(d_ptr, SIGNAL(doneChanged()), SIGNAL(doneChanged()));
    connect(d_ptr, SIGNAL(error(int,QString)), SIGNAL(error(int,QString)));
    connect(d_ptr, SIGNAL(lastIndexChanged()), SIGNAL(lastIndexChanged()));
    connect(d_ptr, SIGNAL(protocolInfoChanged()), SIGNAL(protocolInfoChanged()));
}

BrowseModel &BrowseModel::empty()
{
    return ::empty;
}

void BrowseModel::refresh()
{
    Q_D(BrowseModel);

    d->refresh();
}

QString BrowseModel::protocolInfo() const
{
    Q_D(const BrowseModel);

    return d->protocolInfo();
}

void BrowseModel::setProtocolInfo(const QString &protocolInfo)
{
    Q_D(BrowseModel);

    d->setProtocolInfo(protocolInfo);
}

void BrowseModel::setLastIndex(int index)
{
    Q_D(BrowseModel);

    d->setLastIndex(index);
}

int BrowseModel::lastIndex() const
{
    Q_D(const BrowseModel);

    return d->lastIndex();
}

bool BrowseModel::done() const
{
    Q_D(const BrowseModel);

    return d->done();
}

bool BrowseModel::busy() const
{
    Q_D(const BrowseModel);

    return d->busy();
}
