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

#include <QAbstractListModel>

#include <libgupnp-av/gupnp-av.h>

#include "refptrg.h"

typedef RefPtrG<GUPnPDIDLLiteObject> DIDLLiteObject;

class BrowseModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(bool done READ done NOTIFY doneChanged)
    Q_PROPERTY(QString protocolInfo READ protocolInfo WRITE setProtocolInfo NOTIFY protocolInfoChanged)
public:
    enum BrowseRoles {
        BrowseRoleTitle = Qt::DisplayRole,
        BrowseRoleId = Qt::UserRole + 1,
        BrowseRoleUPnPClass,
        BrowseRoleIcon,
        BrowseRoleURI,
        BrowseRoleType,
        BrowseRoleDetail
    };

    explicit BrowseModel(const ServiceProxy &proxy = ServiceProxy(),
                         const QString      &id = QLatin1String("0"),
                         const QString      &sortCriteria = QLatin1String(""),
                         const QString      &protocolInfo = QLatin1String("*:*:*:*"),
                         QObject            *parent = 0);
    ~BrowseModel();

    // virtual functions from QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    // property getters
    bool busy() const { return m_busy; }
    bool done() const { return m_done; }
    QString protocolInfo() const { return m_protocolInfo; }
    void setProtocolInfo(const QString& protocolInfo);

    // static functions
    static BrowseModel &empty() { return m_empty; }

Q_SIGNALS:
    // property signals
    void busyChanged();
    void doneChanged();
    void protocolInfoChanged();

    void error(int code, const QString& message);
public Q_SLOTS:

    // QML callable functions
    void refresh();

    // Uh, ugly, but there seems no way to call static funcitons from QML
    QString formatTime(long duration);
private Q_SLOTS:
    void onStartBrowse();
    void onBrowseDone(QByteArray result, uint number_returned, uint total_matches);
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

private:
    static BrowseModel m_empty;


    static void on_browse(GUPnPServiceProxy       *proxy,
                          GUPnPServiceProxyAction *action,
                          gpointer                 user_data);

    static void on_didl_object(GUPnPDIDLLiteParser *parser,
                               GUPnPDIDLLiteObject *item,
                               gpointer             user_data);

    QString getCompatibleUri(int index, const QString& protocolInfo) const;

    QList<DIDLLiteObject>    m_data;
    ServiceProxy             m_contentDirectory;
    QString                  m_id;
    guint                    m_currentOffset;
    bool                     m_busy;
    bool                     m_done;
    GUPnPServiceProxyAction *m_action;
    QString                  m_sortCriteria;
    QString                  m_protocolInfo;
};

#endif // BROWSEMODEL_H
