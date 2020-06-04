#include "AbstractItemView.h"
#include <QApplication>
#include <QScrollBar>
#include <QFontMetrics>
#include <QPainter>
#include <QMouseEvent>
#include <QtDebug>
const int ExtraHeight = 3;

//AbstractItemView
AbstractItemView::AbstractItemView(QWidget *parent):QAbstractItemView(parent),
                                idealWidth(0), idealHeight(0),hashIsDirty(false)
{
    setFocusPolicy(Qt::WheelFocus);
    setFont(QApplication::font("QListView"));
    horizontalScrollBar()->setRange(0, 0);
    verticalScrollBar()->setRange(0, 0);

    this->setItemDelegate(new ItemDelegate(this));
    m_itemModel = new AbstractItemModel();
    this->setModel(m_itemModel);

}

AbstractItemView::~AbstractItemView()
{
    if(m_itemModel)
         delete m_itemModel;
     if(pxyModel)
         delete pxyModel;
}

void AbstractItemView::setModel(QAbstractItemModel *model)
{
    QAbstractItemView::setModel(model);
    hashIsDirty = false;
}

QRect AbstractItemView::visualRect(const QModelIndex &index) const
{
    QRect rect;
    if(index.isValid())
        rect = viewportRectForRow(index.row()).toRect();
    return rect;
}

void AbstractItemView::calculateRectsIfNecessary() const
{
    if (!hashIsDirty)
        return;

    const int ExtraWidth = 10;
    QFontMetrics fm(font());
    const int RowHeight = fm.height() + ExtraHeight;
    const int MaxWidth = viewport()->width();
    int minimumWidth = 0;
    int x = 0;
    int y = 0;

    for (int row = 0; row < model()->rowCount(rootIndex()); ++row) {
        QModelIndex index = model()->index(row, 0, rootIndex());
        QString text = model()->data(index).toString();
        int textWidth = fm.horizontalAdvance(text);
        if (!(x == 0 || x + textWidth + ExtraWidth < MaxWidth)) {
            y += RowHeight;
            x = 0;
        }
        else if (x != 0)
            x += ExtraWidth;

        rectForRow[row] = QRectF(x, y, textWidth + ExtraWidth,RowHeight);
        if (textWidth > minimumWidth)
            minimumWidth = textWidth;
        x += textWidth;
    }
    idealWidth = minimumWidth + ExtraWidth;
    idealHeight = y + RowHeight;
    hashIsDirty = false;
    viewport()->update();
}
QRectF AbstractItemView::viewportRectForRow(int row) const
{
    calculateRectsIfNecessary();
    QRectF rect = rectForRow.value(row).toRect();
    if (!rect.isValid())
        return rect;

    return QRectF(rect.x() - horizontalScrollBar()->value(),
                  rect.y() - verticalScrollBar()->value(),
                  rect.width(), rect.height());
}
void AbstractItemView::scrollTo(const QModelIndex &index,
                                QAbstractItemView::ScrollHint)
{
    QRect viewRect = viewport()->rect();
    QRect itemRect = visualRect(index);

    if (itemRect.left() < viewRect.left())
        horizontalScrollBar()->setValue(horizontalScrollBar()->value()
                                        + itemRect.left() - viewRect.left());
    else if (itemRect.right() > viewRect.right())
        horizontalScrollBar()->setValue(horizontalScrollBar()->value()
                                        + qMin(itemRect.right() - viewRect.right(),
                                        itemRect.left() - viewRect.left()));
    if (itemRect.top() < viewRect.top())
        verticalScrollBar()->setValue(verticalScrollBar()->value() +
                                      itemRect.top() - viewRect.top());
    else if (itemRect.bottom() > viewRect.bottom())
        verticalScrollBar()->setValue(verticalScrollBar()->value() +
                                      qMin(itemRect.bottom() - viewRect.bottom(),
                                      itemRect.top() - viewRect.top()));
    viewport()->update();
}
void AbstractItemView::dataChanged(const QModelIndex &topLeft,
                                   const QModelIndex &bottomRight,
                                   const QVector<int> &roles)
{
    hashIsDirty = true;
    QAbstractItemView::dataChanged(topLeft, bottomRight,roles);
}
QModelIndex AbstractItemView::indexAt(const QPoint &point_) const
{
    QPoint point(point_);
    point.rx() += horizontalScrollBar()->value();
    point.ry() += verticalScrollBar()->value();
    calculateRectsIfNecessary();
    QHashIterator<int, QRectF> i(rectForRow);
    while (i.hasNext()) {
        i.next();
        if (i.value().contains(point))
            return model()->index(i.key(), 0, rootIndex());
    }
    return QModelIndex();
}
void AbstractItemView::rowsInserted(const QModelIndex &parent, int start,int end)
{
    hashIsDirty = true;
    QAbstractItemView::rowsInserted(parent, start, end);
}
void AbstractItemView::rowsAboutToBeRemoved(const QModelIndex &parent,int start, int end)
{
    hashIsDirty = true;
    QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
}
QModelIndex AbstractItemView::moveCursor(QAbstractItemView::CursorAction cursorAction,
                                         Qt::KeyboardModifiers)
{
    QModelIndex index = currentIndex();
    if (index.isValid()) {
        if ((cursorAction == MoveLeft && index.row() > 0) ||
            (cursorAction == MoveRight && index.row() + 1 < model()->rowCount()))
        {
            const int offset = (cursorAction == MoveLeft ? -1 : 1);
            index = model()->index(index.row() + offset,
                                   index.column(), index.parent());
        }
        else if ((cursorAction == MoveUp && index.row() > 0) ||
                 (cursorAction == MoveDown && index.row() + 1 < model()->rowCount()))
        {
            QFontMetrics fm(font());
            const int RowHeight = (fm.height() + ExtraHeight) * (cursorAction == MoveUp ? -1 : 1);
            QRect rect = viewportRectForRow(index.row()).toRect();
            QPoint point(rect.center().x(),rect.center().y() + RowHeight);
            while (point.x() >= 0) {
                index = indexAt(point);
                if (index.isValid())
                    break;
                point.rx() -= fm.horizontalAdvance("n");
            }
        }
    }
    return index;
}
int AbstractItemView::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}
int AbstractItemView::verticalOffset() const
{
    return verticalScrollBar()->value();
}
void AbstractItemView::scrollContentsBy(int dx, int dy)
{
    scrollDirtyRegion(dx, dy);
    viewport()->scroll(dx, dy);
}
void AbstractItemView::setSelection(const QRect &rect,
         QFlags<QItemSelectionModel::SelectionFlag> flags)
{
    QRect rectangle = rect.translated(horizontalScrollBar()->value(),
            verticalScrollBar()->value()).normalized();
    calculateRectsIfNecessary();
    QHashIterator<int, QRectF> i(rectForRow);
    int firstRow = model()->rowCount();
    int lastRow = -1;
    while (i.hasNext()) {
        i.next();
        if (i.value().intersects(rectangle)) {
            firstRow = firstRow < i.key() ? firstRow : i.key();
            lastRow  = lastRow  > i.key() ? lastRow  : i.key();
        }
    }
    if (firstRow != model()->rowCount() && lastRow != -1) {
        QItemSelection selection(
                model()->index(firstRow, 0, rootIndex()),
                model()->index(lastRow, 0, rootIndex()));
        selectionModel()->select(selection, flags);
    }
    else {
        QModelIndex invalid;
        QItemSelection selection(invalid, invalid);
        selectionModel()->select(selection, flags);
    }

}
QRegion AbstractItemView::visualRegionForSelection(
        const QItemSelection &selection) const
{
    QRegion region;
    foreach (const QItemSelectionRange &range, selection) {
        for (int row = range.top(); row <= range.bottom(); ++row) {
            for (int column = range.left(); column < range.right(); ++column) {
                QModelIndex index = model()->index(row, column,rootIndex());
                region += visualRect(index);
            }
        }
    }
    return region;
}
void AbstractItemView::paintEvent(QPaintEvent*)
{
    QPainter painter(viewport());
    painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);

    for (int row = 0; row < model()->rowCount(rootIndex()); ++row) {
        QModelIndex index = model()->index(row, 0, rootIndex());
        QRectF      rect  = viewportRectForRow(row);

        if (!rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height())
            continue;

        QStyleOptionViewItem option = viewOptions();
        option.rect = rect.toRect();
        if (selectionModel()->isSelected(index))
            option.state |= QStyle::State_Selected;

        if (currentIndex() == index)
            option.state |= QStyle::State_HasFocus;

        itemDelegate()->paint(&painter, option, index);
        paintOutline(&painter, rect);
    }
}

void AbstractItemView::paintOutline(QPainter *painter,const QRectF &rectangle)
{
    const QRectF rect = rectangle.adjusted(0, 0, -1, -1);
    painter->save();
    painter->setPen(QPen(palette().dark().color(), 0.5));
    painter->drawRect(rect);
    painter->setPen(QPen(Qt::black, 0.5));
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());
    painter->drawLine(rect.bottomRight(), rect.topRight());
    painter->restore();
}

void AbstractItemView::resizeEvent(QResizeEvent*)
{
    hashIsDirty = true;
    calculateRectsIfNecessary();
    updateGeometries();
}

void AbstractItemView::updateGeometries()
{
    QFontMetrics fm(font());
    const int RowHeight = fm.height() + ExtraHeight;
    horizontalScrollBar()->setSingleStep(fm.horizontalAdvance("n"));
    horizontalScrollBar()->setPageStep(viewport()->width());
    horizontalScrollBar()->setRange(0,
            qMax(0, idealWidth - viewport()->width()));
    verticalScrollBar()->setSingleStep(RowHeight);
    verticalScrollBar()->setPageStep(viewport()->height());
    verticalScrollBar()->setRange(0,
            qMax(0, idealHeight - viewport()->height()));
}

void AbstractItemView::mousePressEvent(QMouseEvent *event)
{
    QAbstractItemView::mousePressEvent(event);
    setCurrentIndex(indexAt(event->pos()));
}

//treeView
treeView::treeView(QWidget*parent):QTreeView(parent),m_itemModel(nullptr),pxyModel(nullptr)
{
    this->setItemDelegate(new ItemDelegate(this));
    this->move(600,100);

    m_itemModel = new AbstractItemModel();
    //pxyModel = new ProxyModel();
    //pxyModel->setSourceModel(m_itemModel);

    this->setModel(m_itemModel);
    //this->setSortingEnabled(true);
    //this->sortByColumn(0,Qt::AscendingOrder);
    this->setAllColumnsShowFocus(true);
    this->setDragDropMode(QAbstractItemView::DragDrop);

    //test button
    QPushButton *button = new QPushButton("testItemChange");
    connect(button,SIGNAL(clicked()),this,SLOT(testItemChange()));
    button->move(400,100);
    button->show();

    QPushButton *button_Del = new QPushButton("testItemDel");
    connect(button_Del,SIGNAL(clicked()),this,SLOT(testItemDel()));
    button_Del->move(400,200);
    button_Del->show();

    QPushButton *button_Add = new QPushButton("testItemAdd");
    connect(button_Add,SIGNAL(clicked()),this,SLOT(testItemAdd()));
    button_Add->move(400,300);
    button_Add->show();
}

treeView::~treeView()
{
   if(m_itemModel)
        delete m_itemModel;
    if(pxyModel)
        delete pxyModel;
}

void treeView::testItemChange()
{
    //static_cast<AbstractItemModel*>(pxyModel->sourceModel())->testItemChange();
    m_itemModel->testItemChange();
}
void treeView::testItemDel()
{
    m_itemModel->testItemDel();
}
void treeView::testItemAdd()
{
    m_itemModel->testItemAdd();
}
