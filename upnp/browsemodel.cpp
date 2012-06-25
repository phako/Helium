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

#include <QApplication>

#include "browsemodel.h"
#include "browsemodel_p.h"
#include "settings.h"

static BrowseModel* g_empty;

BrowseModel::BrowseModel(ServiceProxyCall *call, const QString &protocolInfo, QObject *parent)
    : QSortFilterProxyModel(parent)
    , d_ptr(new BrowseModelPrivate(call, protocolInfo, this))
{
    Q_D(BrowseModel);
    setSourceModel(d);
    setFilterRole(d->m_settings.filterInDetails() ? BrowseModelPrivate::BrowseRoleFilter : BrowseModelPrivate::BrowseRoleTitle);
    setFilterCaseSensitivity(Qt::CaseInsensitive);

    connect(d, SIGNAL(busyChanged()), SIGNAL(busyChanged()));
    connect(d, SIGNAL(doneChanged()), SIGNAL(doneChanged()));
    connect(d, SIGNAL(error(int,QString)), SIGNAL(error(int,QString)));
    connect(d, SIGNAL(lastIndexChanged()), SIGNAL(lastIndexChanged()));
    connect(d, SIGNAL(protocolInfoChanged()), SIGNAL(protocolInfoChanged()));
}

BrowseModel &BrowseModel::empty()
{
    if (g_empty == 0) {
        g_empty = new BrowseModel(0, QLatin1String("*:*:*:*"), qApp);
    }

    return *g_empty;
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
