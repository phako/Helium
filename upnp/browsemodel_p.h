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

#ifndef BROWSEMODELPRIVATE_H
#define BROWSEMODELPRIVATE_H

#include <QAbstractListModel>

#include <libgupnp-av/gupnp-av.h>

#include "refptrg.h"

#include "settings.h"
typedef RefPtrG<GUPnPDIDLLiteObject> DIDLLiteObject;

class ServiceProxyCall;
class BrowseModel;
class BrowseModelPrivate : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(bool done READ done NOTIFY doneChanged)
    Q_PROPERTY(QString protocolInfo READ protocolInfo WRITE setProtocolInfo NOTIFY protocolInfoChanged)
    Q_PROPERTY(int lastIndex READ lastIndex WRITE setLastIndex NOTIFY lastIndexChanged)
public:
    enum BrowseRoles {
        BrowseRoleTitle = Qt::DisplayRole,
        BrowseRoleId = Qt::UserRole + 1,
        BrowseRoleUPnPClass,
        BrowseRoleIcon,
        BrowseRoleURI,
        BrowseRoleType,
        BrowseRoleDetail,
        BrowseRoleMetaData,
        BrowseRoleFilter
    };

    explicit BrowseModelPrivate(ServiceProxyCall *call = 0,
                                const QString &protocolInfo = QLatin1String("*:*:*:*"),
                                BrowseModel *parent = 0);
    ~BrowseModelPrivate();

    // virtual functions from QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    // property getters
    bool busy() const { return m_busy; }
    bool done() const { return m_done; }
    QString protocolInfo() const { return m_protocolInfo; }
    int lastIndex() const { return m_lastIndex; }

    // property setters
    void setProtocolInfo(const QString& protocolInfo);
    void setLastIndex(int index);

Q_SIGNALS:
    // property signals
    void busyChanged();
    void doneChanged();
    void protocolInfoChanged();
    void lastIndexChanged();

    void error(int code, const QString& message);
public Q_SLOTS:

    // QML callable functions
    void refresh();

    // Uh, ugly, but there seems no way to call static funcitons from QML
    QString formatTime(long duration);
private Q_SLOTS:
    void onCallReady();
    void setBusy(bool busy) {
        if (m_busy != busy) {
            m_busy = busy;
            Q_EMIT busyChanged();
        }
    }

    void setDone(bool done) {
        if (m_done != done) {
            m_done = done;
            Q_EMIT doneChanged();
        }
    }

    void onFilterInDetailsChanged();

private:
    static BrowseModelPrivate m_empty;

    QString getCompatibleUri(int index, const QString& protocolInfo) const;

    QList<DIDLLiteObject>    m_data;
    guint                    m_currentOffset;
    bool                     m_busy;
    bool                     m_done;
    QString                  m_protocolInfo;
    int                      m_lastIndex;
    ServiceProxyCall * m_call;
    Settings m_settings;
    BrowseModel *q_ptr;
    Q_DECLARE_PUBLIC(BrowseModel)
};

#endif // BROWSEMODELPRIVATE_H
