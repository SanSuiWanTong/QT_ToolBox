#include "AbstractProxyModel.h"
ProxyModel::ProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

bool ProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceRow)
    Q_UNUSED(sourceParent)
   //QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
   return true;
}

bool ProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    QVariant leftData  = sourceModel()->data(source_left);
    QVariant rightData = sourceModel()->data(source_right);
    if(leftData.type() == QVariant::String){
        QString leftString = leftData.toString();
        QString rightString = rightData.toString();
        return QString::localeAwareCompare(leftString, rightString) < 0;
    }
    return false;
}

