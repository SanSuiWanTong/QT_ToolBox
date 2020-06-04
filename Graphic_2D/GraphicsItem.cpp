#include "Graphic_2D/GraphicsItem.h"

GraphicsItem::GraphicsItem(const QPoint &position, GraphicsScene *scene, QGraphicsObject *parent):
    QGraphicsObject(parent),m_pen(Qt::green), m_brush(Qt::NoBrush),
    m_angle(0.0), m_shearHorizontal(0.0),m_shearVertical(0.0)
{
    setFlags(QGraphicsItem::ItemIsSelectable|
             QGraphicsItem::ItemSendsGeometryChanges|
             QGraphicsItem::ItemIsMovable);
    setPos(position);
    setAcceptDrops(true);
    scene->clearSelection();
    scene->addItem(this);
    setSelected(true);
}

void GraphicsItem::setPen(const QPen &pen)
{
    if (isSelected() && m_pen != pen) {
        m_pen = pen;
        update();
        emit dirty();
    }
}

void GraphicsItem::setBrush(const QBrush &brush)
{
    if (isSelected() && m_brush != brush) {
        m_brush = brush;
        update();
        emit dirty();
    }
}

void GraphicsItem::setAngle(double angle)
{
    if (isSelected() && !qFuzzyCompare(m_angle, angle)) {
        m_angle = angle;
        updateTransform();
    }
}

void GraphicsItem::setShear(double shearHorizontal, double shearVertical)
{
    if (isSelected() &&
        (!qFuzzyCompare(m_shearHorizontal, shearHorizontal) ||
         !qFuzzyCompare(m_shearVertical, shearVertical))) {
        m_shearHorizontal = shearHorizontal;
        m_shearVertical = shearVertical;
        updateTransform();
    }
}

void GraphicsItem::updateTransform()
{
    QTransform transform;
    transform.shear(m_shearHorizontal, m_shearVertical);
    transform.rotate(m_angle);
    setTransform(transform);
}

QVariant GraphicsItem::itemChange(GraphicsItemChange change,const QVariant &value)
{
    if (isDirtyChange(change))
        emit dirty();
    return QGraphicsItem::itemChange(change, value);
}

void GraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(m_pen);
    painter->setBrush(m_brush);
    painter->drawPath(shape());
    if(isSelected())
        paintSelectionOutline(painter);
}

void GraphicsItem::paintSelectionOutline(QPainter *painter)
{
    QPen pen(Qt::DashLine);
    pen.setColor(Qt::darkBlue);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(shape());
}

QPainterPath GraphicsItem::shape() const
{
    QPainterPath path;

    QRectF rect(0, 0, 80,50);
    path.addRect(rect);

    return path;
}

QRectF GraphicsItem::boundingRect() const
{
    QRectF rect(0,0,100,200);
    return rect;
}

void GraphicsItem::edit()
{

}

QDataStream &operator<<(QDataStream &out, const GraphicsItem &tItem)
{
    out << tItem.pos() << tItem.angle()
        << tItem.shearHorizontal() << tItem.shearVertical()
        << tItem.zValue()
        << tItem.pen() << tItem.brush();

    return out;
}

QDataStream &operator>>(QDataStream &in, GraphicsItem &tItem)
{
    QPointF position;
    double angle;
    double shearHorizontal;
    double shearVertical;
    double zValue;
    QPen pen;
    QBrush brush;

    in >> position >> angle
       >> shearHorizontal >> shearVertical
       >> zValue
       >> pen >> brush;

    tItem.setPos(position);
    tItem.setAngle(angle);
    tItem.setShear(shearHorizontal, shearVertical);
    tItem.setZValue(zValue);
    tItem.setPen(pen);
    tItem.setBrush(brush);

    return in;
}
