#ifndef ABSTRACTITEMVIEW_H
#define ABSTRACTITEMVIEW_H
#include "./MVC/AbstractItemModel.h"
#include "./MVC/AbstractProxyModel.h"
#include "./MVC/AbstractItemDelegate.h"
#include <QTreeView>
#include <QAbstractItemView>

class AbstractItemView : public QAbstractItemView
{
    Q_OBJECT

public:
    explicit AbstractItemView(QWidget *parent=0);
     ~AbstractItemView();

    void setModel(QAbstractItemModel *model);
    QRect visualRect(const QModelIndex &index) const;
    void scrollTo(const QModelIndex &index,
                  QAbstractItemView::ScrollHint);
    QModelIndex indexAt(const QPoint &point_) const;

protected slots:
    void dataChanged(const QModelIndex &topLeft,
                     const QModelIndex &bottomRight,
                     const QVector<int> &roles = QVector<int>());
    void rowsInserted(const QModelIndex &parent, int start, int end);
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start,
                              int end);
    void updateGeometries();

protected:
    QModelIndex moveCursor(
            QAbstractItemView::CursorAction cursorAction,
            Qt::KeyboardModifiers modifiers);
    bool isIndexHidden(const QModelIndex&) const { return false; }
    int horizontalOffset() const;
    int verticalOffset() const;
    void scrollContentsBy(int dx, int dy);
    void setSelection(const QRect &rect,
            QFlags<QItemSelectionModel::SelectionFlag> flags);
    QRegion visualRegionForSelection(
            const QItemSelection &selection) const;

    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void mousePressEvent(QMouseEvent *event);

private:
    void calculateRectsIfNecessary() const;
    QRectF viewportRectForRow(int row) const;
    void paintOutline(QPainter *painter, const QRectF &rectangle);

    mutable int idealWidth;
    mutable int idealHeight;
    mutable QHash<int, QRectF> rectForRow;
    mutable bool hashIsDirty;

    AbstractItemModel* m_itemModel;
    ProxyModel       * pxyModel;
};

class treeView : public QTreeView
{
    Q_OBJECT
public:
    explicit treeView(QWidget*parent=0);
    ~treeView();

public slots:
    void testItemChange();

private:
    AbstractItemModel* m_itemModel;
    ProxyModel       * pxyModel;
};

#endif // ABSTRACTITEMVIEW_H
