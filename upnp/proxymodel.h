#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QSortFilterProxyModel>
#include "browsemodel.h"

class ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ProxyModel(QObject *parent = 0);
public Q_SLOTS:
    void setSource(QObject *model) {
        setSourceModel(qobject_cast<BrowseModel*>(model));
        dataChanged(index(0, 0), index(rowCount(), 0));
    }
};

#endif // PROXYMODEL_H
