#ifndef ABSTRACTITEMMODEL_H
#define ABSTRACTITEMMODEL_H
#include <QAbstractItemModel>
#include "./MVC/AbstractItemData.h"

const QString MimeType   = "text/plain";
const int MaxCompression = 9;
class AbstractItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit AbstractItemModel(QObject *parent = 0);
    ~AbstractItemModel();

    QVariant     data       (const QModelIndex &index, int role = Qt::DisplayRole)            const;
    QModelIndex  index      (int row, int column, const QModelIndex &parent = QModelIndex())  const;
    QModelIndex  parent     (const QModelIndex &index)                                        const;
    int          rowCount   (const QModelIndex &parent = QModelIndex())                       const;
    int          columnCount(const QModelIndex &parent = QModelIndex())                       const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void          announceItemChanged(AbstractItemData *item);
    void          testItemChange();
    QVariant      headerData(int section,
                             Qt::Orientation orientation,
                             int role=Qt::DisplayRole) const;

    bool          setData(const QModelIndex &index, const QVariant &value,int role=Qt::EditRole);
    bool          removeRows(int row, int count,const QModelIndex &parent=QModelIndex());

    //drag
    Qt::DropActions supportedDragActions() const { return Qt::MoveAction; }
    Qt::DropActions supportedDropActions() const { return Qt::MoveAction; }
    QStringList     mimeTypes()            const{ return QStringList()<<MimeType;}
    QMimeData *     mimeData(const QModelIndexList &indexes) const;
    bool            dropMimeData(const QMimeData *mimeData,
                                 Qt::DropAction action,
                                 int row,
                                 int column,
                                 const QModelIndex &parent);
    void saveDragItem(AbstractItemData *item)const { dragItem = item; }
    bool removeStartDropItem();
    //
public slots:
    void onItemDataChange(AbstractItemData*);

private:
    AbstractItemData* itemForIndex(const QModelIndex &index) const;
    AbstractItemData* rootItem;

    mutable AbstractItemData * dragItem;
    bool bDrop = false;
};

#endif // ABSTRACTITEMMODEL_H
