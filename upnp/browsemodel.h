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

#ifndef BROWSEMODEL_H
#define BROWSEMODEL_H

#include <QAbstractListModel>

#include <libgupnp-av/gupnp-av.h>

#include "refptrg.h"

typedef RefPtrG<GUPnPDIDLLiteObject> DIDLLiteObject;

class BrowseModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(bool done READ done NOTIFY doneChanged)
public:
    enum BrowseRoles {
        BrowseRoleTitle = Qt::DisplayRole,
        BrowseRoleId = Qt::UserRole + 1,
        BrowseRoleUPnPClass,
        BrowseRoleDuration,
        BrowseRoleSize,
        BrowseRoleIcon,
        BrowseRoleURI,
        BrowseRoleType
    };

    explicit BrowseModel(const ServiceProxy &proxy = ServiceProxy(),
                         const QString      &id = QLatin1String("0"),
                         QObject            *parent = 0);

    // virtual functions from QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool busy() const { return m_busy; }
    bool done() const { return m_done; }
    static BrowseModel &empty() { return m_empty; }

Q_SIGNALS:
    void busyChanged();
    void doneChanged();
public Q_SLOTS:
    void refresh();
private Q_SLOTS:
    void onStartBrowse();
private:
    static BrowseModel m_empty;

    void setBusy(bool busy) {
        if (m_busy != busy) {
            m_busy = busy;
            Q_EMIT busyChanged();
        }
    }

    void setDone(bool done) {
        if (m_done != done) {
            m_done = done;
        }

        Q_EMIT doneChanged();
    }

    static void on_browse(GUPnPServiceProxy       *proxy,
                          GUPnPServiceProxyAction *action,
                          gpointer                 user_data);

    static void on_didl_object(GUPnPDIDLLiteParser *parser,
                               GUPnPDIDLLiteObject *item,
                               gpointer             user_data);

    QList<DIDLLiteObject> m_data;
    ServiceProxy m_contentDirectory;
    QString m_id;
    guint m_currentOffset;
    bool m_busy;
    bool m_done;
};

#endif // BROWSEMODEL_H
