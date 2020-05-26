#ifndef CENTERCONTROL_H
#define CENTERCONTROL_H
#include <QObject>
#include <QJSValue>
#include "mainwindow.h"
class centerControl: public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE centerControl();
    Q_INVOKABLE void createMainWindow();
    Q_INVOKABLE void createQcharWidget();

    Q_INVOKABLE void createQTreeWidget();
    Q_INVOKABLE void createQTableWidget();
    Q_INVOKABLE void createQListWidget();

    Q_INVOKABLE void createQTreeView();
    Q_INVOKABLE void createQTableView();
    Q_INVOKABLE void createQListView();

    Q_INVOKABLE void createMVCView();

    Q_INVOKABLE void printMsg(QJSValue msg);

    //Q_INVOKABLE QMainWindow* getMainWindow(){ return m_pMainWindow; }
signals:
    void testSignal();

private:
    QMainWindow* m_pMainWindow = nullptr;
};

#endif // CENTERCONTROL_H
