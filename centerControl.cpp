#include "centerControl.h"
#include "QcharWidget.h"
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

void centerControl::printMsg(QJSValue msg)
{
    qDebug()<<msg.toString();
}
