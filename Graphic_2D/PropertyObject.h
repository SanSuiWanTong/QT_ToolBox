#ifndef PROPERTYOBJECT_H
#define PROPERTYOBJECT_H

#include <QObject>

class PropertyObject : public QObject
{
    Q_OBJECT
public:
    explicit PropertyObject(QObject *parent = nullptr);

signals:

};

#endif // PROPERTYOBJECT_H
