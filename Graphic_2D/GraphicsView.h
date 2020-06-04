#ifndef GRAPHICVIEW_H
#define GRAPHICVIEW_H
#include "3rdparty/widgetPropertySet/penwidget.hpp"
#include "3rdparty/widgetPropertySet/brushwidget.hpp"
#include "3rdparty/widgetPropertySet/transformwidget.hpp"

#include "Graphic_2D/GraphicsScene.h"
#include <QGraphicsView>
#include <QPainter>
#include <QPrinter>
#include <QWheelEvent>
#include <cmath>

//辅助功能测试
#include <QToolBar>
class myToolBar :public QToolBar
{
    Q_OBJECT
public:
    explicit myToolBar(QWidget*parent=0);

};

//
class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QWidget *parent=0);

    void readItems(QDataStream &in, int offset=0, bool select=false);
    void writeItems(QDataStream &out, const QList<QGraphicsItem*> &items);
    void copyItems();
    void pasteItems();
    void fileExport();
    void exportImage(const QString &filename);
    void exportSvg(const QString &filename);
    void clear();
    void connectItem(QObject *item);    
    void viewShowGrid(bool on);

    //
    void addColorItem();
    void addRobotItem();

public slots:
    void zoomIn() { scaleBy(1.1); }
    void zoomOut() { scaleBy(1.0 / 1.1); }

    bool fileSave();
    void loadFile();
    void addItem(int type);
    void selectionChanged();

    void actionSlot(QAction *action);

protected:
    void wheelEvent(QWheelEvent *event)
        { scaleBy(std::pow(4.0 / 3.0, (event->angleDelta().y() / 240.0))); }

private:
    void scaleBy(double factor) { scale(factor, factor); }
    QPoint position();

    myToolBar          * pToolBar;
    GraphicsScene      * scene;
    QGraphicsItemGroup * gridGroup;//参考网格线
    QPrinter           * printer;
    QPoint               previousPoint;

    TransformWidget * transformWidget;
    BrushWidget     * brushWidget;
    PenWidget       * penWidget;

    int addOffset;
    int pasteOffset;
};

#endif // GRAPHICVIEW_H
