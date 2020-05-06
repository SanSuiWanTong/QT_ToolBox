#ifndef CENTERCONTROL_H
#define CENTERCONTROL_H
#include <QObject>
#include "mainwindow.h"
class centerControl: public QObject
{
    Q_OBJECT

public:
    centerControl();    

    Q_INVOKABLE void createMainWindow();
    Q_INVOKABLE QMainWindow* getMainWindow(){ return m_pMainWindow; }

signals:
    void testSignal();

private:
    QMainWindow* m_pMainWindow = nullptr;
};

#endif // CENTERCONTROL_H
