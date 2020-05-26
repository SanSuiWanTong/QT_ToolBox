#include "centerControl.h"
#include "QcharWidget.h"
#include "./MVC/QTreeTableListWidgetDemo.h"
#include "./MVC/AbstractItemView.h"
#include "./MVC/AbstractItemWidget.h"
#include <QDebug>

centerControl::centerControl()
{
}

void centerControl::createMainWindow()
{
    m_pMainWindow = new MainWindow();
    m_pMainWindow->show();
    emit testSignal();
}
void centerControl::createQcharWidget()
{
    QcharWidget* w = new QcharWidget();
    w->show();
}
//
void centerControl::createQTreeWidget()
{
    QTreeWidget * w = new DemoQTreeWidget();
    w->show();
}
void centerControl::createQTableWidget()
{
    QTableWidget * w = new DemoQTableWidget();
    w->show();
}
void centerControl::createQListWidget()
{
    QWidget * w = new DemoQListWidget();
    w->show();
}
//
void centerControl::createQTreeView()
{
    auto * w = new DemoQTreeView();
    w->show();
}
void centerControl::createQTableView()
{
    auto * w = new DemoQTableView();
    w->show();
}
void centerControl::createQListView()
{
    auto * w = new DemoQListView();
    w->show();
}

//
void centerControl::createMVCView()
{
    //test 1
    auto *t = new treeView();
    //test 2
    //auto *t = new AbstractItemView();
    //test 3
    //auto *t = new AbstractItemWidget();
    t->show();
}

//
void centerControl::printMsg(QJSValue msg)
{
    qDebug()<<msg.toString();
}
