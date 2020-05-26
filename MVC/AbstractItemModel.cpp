#include "AbstractItemModel.h"
#include <QDebug>
#include <QObject>
#include <QMimeData>

AbstractItemModel::AbstractItemModel(QObject *parent)
                   : QAbstractItemModel(parent),rootItem(new AbstractItemData())
{
    rootItem->load();
    connect(rootItem,SIGNAL(itemDataChanged(AbstractItemData*)),this,SLOT(onItemDataChange(AbstractItemData*)));
}

AbstractItemModel::~AbstractItemModel()
{
    delete rootItem;
}

QVariant     AbstractItemModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if (AbstractItemData *item = itemForIndex(index)) {
        if(Qt::DisplayRole == role){
            return item->getData(index.column());
        }
        if (role == Qt::CheckStateRole){
            return item->getChecked() ? Qt::Checked : Qt::Unchecked;
        }
        if (role == Qt::TextAlignmentRole) {
            return QVariant();
        }
        if (role == Qt::DecorationRole){
            return QVariant();
        }
    }
    return QVariant();
}

/* 树的子项不显示是index 没有创建，树的column 的列号必须是0 */
QModelIndex  AbstractItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    AbstractItemData *parentItem = parent.isValid() ? itemForIndex(parent) : rootItem;

    if (AbstractItemData *childItem = parentItem->childAt(row))
        return createIndex(row, column, childItem);

    return QModelIndex();
}
/* Returns the parent of the model item with the given index */
QModelIndex  AbstractItemModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    AbstractItemData *childItem = itemForIndex(index);
    AbstractItemData *parentItem = childItem->parent();

    if (parentItem == rootItem||!parentItem)
        return QModelIndex();

    int row = parentItem->rowOfChild();             //this item in the parentItem's row
    return createIndex(row, 0, parentItem);
}

int          AbstractItemModel::rowCount(const QModelIndex &parent) const
{
    AbstractItemData * parentItem;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = itemForIndex(parent);

    return parentItem ? parentItem->childCount() : 0;
}

int          AbstractItemModel::columnCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        rootItem->columnCount();

    AbstractItemData *parentItem = itemForIndex(parent);
    return parentItem ? parentItem->columnCount() : 0;
}

Qt::ItemFlags AbstractItemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractItemModel::flags(index);
    if (index.isValid()){
        theFlags |= Qt::ItemIsSelectable   |Qt::ItemIsEnabled|
                    Qt::ItemIsUserCheckable|Qt::ItemIsEditable|
                    Qt::ItemIsDragEnabled  |Qt::ItemIsDropEnabled;
    }
    return theFlags;
}

bool AbstractItemModel::setData(const QModelIndex &index, const QVariant &value,int role)
{
    if (!index.isValid())
        return false;
    if (auto *item = itemForIndex(index)) {
        if (role == Qt::EditRole){
            item->setData(index.column(),value);            
        }else if (role == Qt::CheckStateRole) {
            item->setChecked(value.toBool());
            emit dataChanged(this->index(0,0,index),this->index(item->childCount(),0,index));
        }
        else
            return false;
        emit dataChanged(index,index);
        return true;
    }
    return false;
}

AbstractItemData *AbstractItemModel::itemForIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        if (AbstractItemData *item = static_cast<AbstractItemData*>(index.internalPointer()))
            return item;
    }
    return rootItem;
}

void AbstractItemModel::announceItemChanged(AbstractItemData *item)
{
    if (item == rootItem)
        return;

    AbstractItemData *parent = item->parent();
    Q_ASSERT(parent);
    int row = item->rowOfChild();
    QModelIndex startIndex = createIndex(row,0,item);
    QModelIndex endIndex   = createIndex(row,item->columnCount(),item);

    emit dataChanged(startIndex, endIndex);
    // Update the parent & parent's parent etc too
    announceItemChanged(parent);
}

void AbstractItemModel::onItemDataChange(AbstractItemData* item)
{
     int row = item->rowOfChild();
     QModelIndex startIndex = createIndex(row,0,item);
     QModelIndex endIndex   = createIndex(row,item->columnCount(),item);
     emit dataChanged(startIndex, endIndex);
}
void AbstractItemModel::testItemChange()
{
    this->rootItem->testItemChange();
}
QVariant AbstractItemModel::headerData(int section,Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal) {
        return QString("%1_%2").arg("col").arg(section);
    }
    return QVariant();
}
QMimeData *AbstractItemModel::mimeData(const QModelIndexList &indexes) const
{
    Q_ASSERT(indexes.count());
    if (indexes.count() != 1)
        return 0;   
    if (auto dragItem = itemForIndex(indexes.at(0))) {
        saveDragItem(dragItem);
        QMimeData *mimeData = new QMimeData;
        QByteArray itemData;       
        QTextStream out(&itemData,QIODevice::WriteOnly);
        dragItem->getItemAndChildStream(out);
        mimeData->setData(MimeType, qCompress(itemData,MaxCompression));
        return mimeData;
    }
    return 0;
}
bool AbstractItemModel::dropMimeData(const QMimeData *mimeData,
                                     Qt::DropAction action,
                                     int row,
                                     int column,
                                     const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;
    if ((action != Qt::MoveAction)||(column > 0) ||(!mimeData) ||(!mimeData->hasFormat(MimeType)))
        return false;

    if (auto *item = itemForIndex(parent)) {
        QByteArray itemData = qUncompress(mimeData->data(MimeType));
        if (row == -1)
            row = parent.isValid() ? parent.row() : rootItem->childCount();
        beginInsertRows(parent, row, row);
        QTextStream stream(&itemData);        
        item->setItemAndChildStream(&stream,row,column);        
        endInsertRows();
        removeStartDropItem();
        return true;
    }    
    return false;
}
bool AbstractItemModel::removeStartDropItem()
{
    auto parent = this->dragItem->parent();
    int  row    = this->dragItem->rowOfChild();
    auto index  = createIndex(row,0,this->dragItem);

    beginRemoveRows(index, row, row);
    parent->removeChild(row);
    endRemoveRows();
    bDrop = true;

    return true;
}
bool  AbstractItemModel::removeRows(int row, int count, const QModelIndex &parent)
{
     if (!rootItem)
         return false;
     if(bDrop)
     {
         this->dragItem=nullptr;
         bDrop = false;
         return false;
     }

     auto *item = parent.isValid() ? itemForIndex(parent) : rootItem;
     int endRow = row + count - 1;
     beginRemoveRows(parent, row, endRow);
     for (int i = row; i <= endRow; ++i)
         item->removeChild(i);
     endRemoveRows();

     return true;
 }
