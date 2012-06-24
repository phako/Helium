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

#ifndef BROWSEMODEL_H
#define BROWSEMODEL_H

#include <QtGui/QSortFilterProxyModel>

class BrowseModelPrivate;
class ServiceProxyCall;
class BrowseModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(bool done READ done NOTIFY doneChanged)
    Q_PROPERTY(QString protocolInfo READ protocolInfo WRITE setProtocolInfo NOTIFY protocolInfoChanged)
    Q_PROPERTY(int lastIndex READ lastIndex WRITE setLastIndex NOTIFY lastIndexChanged)
public:
    explicit BrowseModel(ServiceProxyCall *call = 0,
                         const QString &protocolInfo = QLatin1String("*:*:*:*"),
                         QObject       *parent = 0);

    static BrowseModel &empty();
    Q_INVOKABLE void refresh();

    // property getters
    bool busy() const;
    bool done() const;
    QString protocolInfo() const;
    int lastIndex() const;

    // property setters
    void setProtocolInfo(const QString &protocolInfo);
    void setLastIndex(int index);

Q_SIGNALS:
    // property signals
    void busyChanged();
    void doneChanged();
    void protocolInfoChanged();
    void lastIndexChanged();

    void error(int code, const QString &message);

public Q_SLOTS:
private:
    BrowseModelPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(BrowseModel)
};

#endif // BROWSEMODEL_H
