#ifndef GRAPHICSITEM_H
#define GRAPHICSITEM_H
#include "Graphic_2D/GraphicsScene.h"
#include <QGraphicsObject>
#include <QDataStream>
#include <QPen>
#include <QBrush>
#include <QPainter>

enum ShapeType { NON,Text,Line,Rect,Circle,Ellipse,Polygon };

class GraphicsItem : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
    Q_PROPERTY(QPen pen READ pen WRITE setPen)
    Q_PROPERTY(double angle READ angle WRITE setAngle)
    Q_PROPERTY(double shearHorizontal READ shearHorizontal WRITE setShearHorizontal)
    Q_PROPERTY(double shearVertical READ shearVertical WRITE setShearVertical)

public:
    explicit GraphicsItem(const QPoint &position, GraphicsScene *scene,QGraphicsObject*parent=0);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    QPainterPath shape() const;
    int type() const { return Type; }
    void setType(const ShapeType type);

    double angle() const { return m_angle; }
    double shearHorizontal() const { return m_shearHorizontal; }
    double shearVertical() const { return m_shearVertical; }

    QPen pen() const { return m_pen; }
    QBrush brush() const { return m_brush; }

    bool isDirtyChange(QGraphicsItem::GraphicsItemChange change)
    {
        return (change == QGraphicsItem::ItemPositionChange     ||
                change == QGraphicsItem::ItemPositionHasChanged ||
                change == QGraphicsItem::ItemTransformChange    ||
                change == QGraphicsItem::ItemTransformHasChanged);
    }

signals:
    void dirty();

public slots:
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAngle(double angle);
    void setShearHorizontal(double shearHorizontal)
        { setShear(shearHorizontal, m_shearVertical); }
    void setShearVertical(double shearVertical)
        { setShear(m_shearHorizontal, shearVertical); }
    void setShear(double shearHorizontal, double shearVertical);
    void edit();

protected:
    QVariant itemChange(GraphicsItemChange change,const QVariant &value);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*)  { edit(); }
    void contextMenuEvent(QGraphicsSceneContextMenuEvent*) { edit(); }

private:
    void updateTransform();
    void paintSelectionOutline(QPainter *painter);

    QPen   m_pen;
    QBrush m_brush;
    double m_angle;
    double m_shearHorizontal;
    double m_shearVertical;
    int  Type = ShapeType::NON;
};

QDataStream &operator<<(QDataStream &out, const GraphicsItem &tItem);
QDataStream &operator>>(QDataStream &in ,       GraphicsItem &tItem);

#endif // GRAPHICSITEM_H
