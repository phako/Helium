#include <QStringListModel>

#include "proxymodel.h"

ProxyModel::ProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
    qDebug("New Proxymodel");
    setSourceModel(new QStringListModel());
}
