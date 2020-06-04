#include "GraphicsView.h"
#include "3rdparty/aqp.hpp"
#include "Graphic_2D/GraphicsItem.h"
#include "Graphic_2D/animation/coloritem.h"
#include "Graphic_2D/animation/robot.h"
#include <QFile>
#include <QPoint>
#include <QFileDialog>
#include <QMimeData>
#include <QClipboard>
#include <QImageWriter>
#include <QSvgGenerator>
#include <QDebug>

const int OffsetIncrement = 5;
const QString MimeType = "sansuiwantong";
enum ActionID { NOP,addRect,fileSave,fileLoad,copy,paste,exportImage,exportSvg,
                colorItem,robotItem};

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent),
    gridGroup(0), addOffset(OffsetIncrement), pasteOffset(OffsetIncrement)
{
    setDragMode(RubberBandDrag);
    setRenderHints(QPainter::Antialiasing|
                   QPainter::TextAntialiasing);
    //set style widget
    penWidget       = new PenWidget();
    brushWidget     = new BrushWidget();
    transformWidget = new TransformWidget();

    penWidget->show();
    brushWidget->show();
    transformWidget->show();

    //auxiliary tool
    pToolBar = new myToolBar();
    connect(pToolBar,SIGNAL(actionTriggered(QAction *)),this,SLOT(actionSlot(QAction *)));
    pToolBar->show();
    //
    printer = new QPrinter(QPrinter::HighResolution);
    //
    scene = new GraphicsScene(this);
    scene->setSceneRect(printer->pageLayout().paintRect() );    
    connect(scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    setScene(scene);
    //
    viewShowGrid(true);
}

bool GraphicsView::fileSave()
{
    QString filename = QFileDialog::getSaveFileName(this,
            tr("%1 - Save As").arg(QApplication::applicationName()),
            ".", tr("SanSuiWanTong (*.pd)"));
    if (filename.isEmpty())
        return false;
    if (!filename.toLower().endsWith(".pd"))
        filename += ".pd";

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    QDataStream out(&file);
    writeItems(out, scene->items());
    file.close();

    return true;
}

void GraphicsView::loadFile()
{
    QString filename = QFileDialog::getOpenFileName(this,
            tr("%1 - LoadFile").arg(QApplication::applicationName()),
            ".", tr("SanSuiWanTong (*.pd)"));
    if (filename.isEmpty())
        return ;

    QFile file(filename);
    QDataStream in;
    in.setDevice(&file);
    if (!file.open(QIODevice::ReadOnly)) {
        AQP::warning(this, tr("Error"), tr("Failed to open file: %1")
                     .arg(file.errorString()));
        return;
    }

    clear();
    readItems(in);
    viewShowGrid(true);
}

void GraphicsView::clear()
{
    scene->clear();   
}

void GraphicsView::writeItems(QDataStream &out, const QList<QGraphicsItem *> &items)
{
    foreach (QGraphicsItem *item, items) {
        if (item == gridGroup || item->group() == gridGroup)
            continue;
        out << *static_cast<GraphicsItem*>(item);
    }
}

void GraphicsView::readItems(QDataStream &in, int offset, bool select)
{
    Q_UNUSED(select)
    GraphicsItem *item = 0;
    while (!in.atEnd()) {
        GraphicsItem *tItem = new GraphicsItem(QPoint(), scene);
        in >> *tItem;
        item = tItem;

        if (item)
            item->moveBy(offset, offset);
            connectItem(item);
    }
}

void GraphicsView::connectItem(QObject *item)
{
    //connect(item, SIGNAL(dirty()), this, SLOT(setDirty()));
    const QMetaObject *metaObject = item->metaObject();
    if (metaObject->indexOfProperty("brush") > -1)
        connect(brushWidget, SIGNAL(brushChanged(const QBrush&)),
                item, SLOT(setBrush(const QBrush&)));
    if (metaObject->indexOfProperty("pen") > -1)
        connect(penWidget, SIGNAL(penChanged(const QPen&)),
                item, SLOT(setPen(const QPen&)));
    if (metaObject->indexOfProperty("angle") > -1) {
        connect(transformWidget, SIGNAL(angleChanged(double)),
                item, SLOT(setAngle(double)));
        connect(transformWidget, SIGNAL(shearChanged(double, double)),
                item, SLOT(setShear(double, double)));
    }
}

void GraphicsView::actionSlot(QAction *action)
{
    if(!action) return;
    int type = action->data().toInt();
    switch(type){
    case ActionID::addRect:
        addItem(type);
        break;
    case ActionID::fileSave:
        fileSave();
        break;
    case ActionID::fileLoad:
        loadFile();
        break;
    case ActionID::copy:
        copyItems();
        break;
    case ActionID::paste:
        pasteItems();
        break;
    case ActionID::exportImage:
    case ActionID::exportSvg:
        fileExport();
        break;
    case ActionID::colorItem:
        addColorItem();
        break;
    case ActionID::robotItem:
        addRobotItem();
        break;

    default:
        break;
    }
}

void GraphicsView::addItem(int type)
{
    GraphicsItem *item = new GraphicsItem(position(), scene);
    if (item) {
        connectItem(item);
        item->moveBy(OffsetIncrement, OffsetIncrement);
    }
}

QPoint GraphicsView::position()
{
    QPoint point = mapFromGlobal(QCursor::pos());
    if (!this->geometry().contains(point)) {
        point = previousPoint.isNull()
                ? this->pos() + QPoint(10, 10) : previousPoint;
    }
    if (!previousPoint.isNull() && point == previousPoint) {
        point += QPoint(addOffset, addOffset);
        addOffset += OffsetIncrement;
    }
    else {
        addOffset = OffsetIncrement;
        previousPoint = point;
    }
    return this->mapToScene(point - this->pos()).toPoint();
}

void GraphicsView::viewShowGrid(bool on)
{
    if (!gridGroup) {
        const int GridSize = 40;
        QPen pen(QColor(175, 175, 175, 127));
        gridGroup = new QGraphicsItemGroup;
        const int MaxX = static_cast<int>(std::ceil(scene->width())
                / GridSize) * GridSize;
        const int MaxY = static_cast<int>(std::ceil(scene->height())
                / GridSize) * GridSize;
        for (int x = 0; x <= MaxX; x += GridSize) {
            QGraphicsLineItem *item = new QGraphicsLineItem(x, 0, x,
                                                            MaxY);
            item->setPen(pen);
            item->setZValue(std::numeric_limits<int>::min());
            gridGroup->addToGroup(item);
        }
        for (int y = 0; y <= MaxY; y += GridSize) {
            QGraphicsLineItem *item = new QGraphicsLineItem(0, y,
                                                            MaxX, y);
            item->setPen(pen);
            item->setZValue(-1000);
            gridGroup->addToGroup(item);
        }
        scene->addItem(gridGroup);
    }
    gridGroup->setVisible(on);
}

void GraphicsView::addColorItem()
{
    for(int i=0;i<5;i++){
        auto colorItem = new ColorItem();
        colorItem->setPos(100,200+i*50);
        scene->addItem(colorItem);
    }
}
void GraphicsView::addRobotItem()
{
    auto tItem = new Robot();
    tItem->setPos(200,300);
    scene->addItem(tItem);
}
void GraphicsView::copyItems()
{
    QList<QGraphicsItem*> items = scene->selectedItems();
    if (items.isEmpty())
        return;
    QByteArray copiedItems;
    QDataStream out(&copiedItems, QIODevice::WriteOnly);
    writeItems(out, items);
    QMimeData *mimeData = new QMimeData;
    mimeData->setData(MimeType, copiedItems);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData);
    pasteOffset += OffsetIncrement;
}

void GraphicsView::pasteItems()
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if (!mimeData)
        return;

    if (mimeData->hasFormat(MimeType)) {
        QByteArray copiedItems = mimeData->data(MimeType);
        QDataStream in(&copiedItems, QIODevice::ReadOnly);
        readItems(in, pasteOffset, true);
    }
}

void GraphicsView::fileExport()
{
    QString suffixes = AQP::filenameFilter(tr("Bitmap image"),
            QImageWriter::supportedImageFormats());
    suffixes += tr(";;Vector image (*.svg)");
    const QString filename = QFileDialog::getSaveFileName(this,
            tr("%1 - Export").arg(QApplication::applicationName()),
            ".", suffixes);
    if (filename.isEmpty())
        return;
    if (filename.toLower().endsWith(".svg"))
        exportSvg(filename);
    else
        exportImage(filename);
}
void GraphicsView::exportImage(const QString &filename)
{
    auto rect = printer->pageLayout().paintRect();
    QImage image(rect.size().toSize(),QImage::Format_ARGB32);
    image.fill(QColor(Qt::white).rgb());
    {
        QPainter painter(&image);
        painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);
        scene->render(&painter);
    }//not need Qpainter.end()
    if (image.save(filename))
        AQP::information(this, tr("Info"), tr("Save finish: %1")
                                        .arg(filename));
    else
        AQP::warning(this, tr("Error"), tr("Failed to export: %1")
                                        .arg(filename));
}

void GraphicsView::exportSvg(const QString &filename)
{
    QSvgGenerator svg;
    svg.setFileName(filename);
    svg.setSize(printer->paperSize(QPrinter::Point).toSize());
    {
        QPainter painter(&svg);
        scene->render(&painter);
    }
    AQP::information(this, tr("Info"), tr("Save finish: %1")
                                    .arg(filename));
}

void GraphicsView::selectionChanged()
{
    QList<QGraphicsItem*> items = scene->selectedItems();
    if (items.count() == 1) {
        if (QObject *item = dynamic_cast<QObject*>(items.at(0))) {
            if (item->property("brush").isValid())
                brushWidget->setBrush(item->property("brush").value<QBrush>());
            if (item->property("pen").isValid())
                penWidget->setPen(item->property("pen").value<QPen>());
            if (item->property("angle").isValid()) {
                transformWidget->setAngle(item->property("angle").toDouble());
                transformWidget->setShear(item->property("shearHorizontal").toDouble(),
                                          item->property("shearVertical").toDouble());
            }
        }
    }
}

//--- 辅助功能测试 ---
myToolBar::myToolBar(QWidget*parent):QToolBar(parent)
{
    //rect
    const QSize size(24, 24);
    const int margin = 4;
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPen pen(Qt::darkBlue, 2, Qt::SolidLine);
    painter.setPen(pen);
    painter.drawRect(margin, margin, size.width()-margin*2, size.height()-margin*3);
    QAction *addRectItem = new QAction(pixmap,"rectangle",this);
    addRectItem->setData(ActionID::addRect);
    this->addAction(addRectItem);

    pixmap.fill(Qt::transparent);
    painter.drawEllipse(margin, margin, size.width()-margin*2, size.height()-margin*3);
    QAction *addEllipsetItem = new QAction(pixmap,"Ellipset",this);
    addEllipsetItem->setData(ActionID::NOP);
    this->addAction(addEllipsetItem);

    //fileSave
    QAction * fileSave = new QAction("fileSave",this);
    fileSave->setData(ActionID::fileSave);
    this->addAction(fileSave);

    //fileLoad
    QAction* fileLoad = new QAction("fileLoad",this);
    fileLoad->setData(ActionID::fileLoad);
    this->addAction(fileLoad);

    //copy
    QAction* copyItem = new QAction("copyItem",this);
    copyItem->setData(ActionID::copy);
    this->addAction(copyItem);

    //paste
    QAction* pasteItem = new QAction("pasteItem",this);
    pasteItem->setData(ActionID::paste);
    this->addAction(pasteItem);

    //exportImage
    QAction* exportImage = new QAction("exportImage",this);
    exportImage->setData(ActionID::exportImage);
    this->addAction(exportImage);

    //exportSvg
    QAction* exportSvg = new QAction("exportSvg",this);
    exportSvg->setData(ActionID::exportSvg);
    this->addAction(exportSvg);

    //colorItem
    QAction* colorItem = new QAction("colorItem",this);
    colorItem->setData(ActionID::colorItem);
    this->addAction(colorItem);

    //robotItem
    QAction* robotItem = new QAction("robotItem",this);
    robotItem->setData(ActionID::robotItem);
    this->addAction(robotItem);
}
