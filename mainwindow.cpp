#include "mainwindow.h"
#include <QPushButton>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QPushButton* bt = new QPushButton(this);
    bt->setText("SendSignaltoJS");

    connect(bt,SIGNAL(clicked()),this,SLOT(sendSignal()));

}

MainWindow::~MainWindow()
{
}

void MainWindow::sendSignal(){
    qDebug()<<"signal send";
    emit testSignal(5);
}
