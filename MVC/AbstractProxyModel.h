#ifndef ABSTRACTPROXYMODEL_H
#define ABSTRACTPROXYMODEL_H
#include <QSortFilterProxyModel>

class ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ProxyModel(QObject *parent=0);

protected:
    bool filterAcceptsRow(int sourceRow,const QModelIndex &sourceParent)           const;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;

};

#endif // ABSTRACTPROXYMODEL_H
