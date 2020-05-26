#include "QTreeTableListWidgetDemo.h"

DemoQTreeWidget::DemoQTreeWidget(QWidget * parent):QTreeWidget(parent)
{
    initDemo();
}

DemoQTreeWidget::~DemoQTreeWidget()
{

}

void DemoQTreeWidget::initDemo()
{
    this->setColumnCount(2);
    QList<QTreeWidgetItem *> items;
    for (int i = 0; i < 10; ++i)
    {
        auto item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("item: %1").arg(i)));
        items.append(item);
        for (int i = 0; i < 3; ++i)
            items.append(new QTreeWidgetItem(item, QStringList(QString("item: %1").arg(i))));
    }
    this->insertTopLevelItems(0, items);
}

//Table
DemoQTableWidget::DemoQTableWidget(QWidget * parent):QTableWidget(parent)
{
    initDemo();
}

DemoQTableWidget::~DemoQTableWidget()
{

}

void DemoQTableWidget::initDemo()
{
    this->setRowCount(10);
    this->setColumnCount(5);
    for(int row = 0;row < 10;row++){
        for(int col = 0;col < 10;col++)
        {
            QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg((row+1)*(col+1)));
            this->setItem(row, col, newItem);
        }
    }
}

//List
DemoQListWidget::DemoQListWidget(QWidget * parent):QListWidget(parent)
{
    initDemo();
}

DemoQListWidget::~DemoQListWidget()
{

}

void DemoQListWidget::initDemo()
{
    for(int i = 0; i<10; i++){
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(QString("%1").arg(i));
        this->insertItem(i, newItem);
    }
}
//Model
QemoQFileModel::QemoQFileModel(QFileSystemModel * parent):QFileSystemModel(parent)
{
    initDemo();
}

QemoQFileModel::~QemoQFileModel()
{

}

void QemoQFileModel::initDemo()
{
   this->setRootPath(QDir::currentPath());
}
//QTreeView
DemoQTreeView::DemoQTreeView(QTreeView * parent):QTreeView(parent)
{
    initDemo();
}

DemoQTreeView::~DemoQTreeView()
{

}

void DemoQTreeView::initDemo()
{
    auto model = new QemoQFileModel();
    model->initDemo();
    this->setModel(model);


}
//QTableView
DemoQTableView::DemoQTableView(QTableView * parent):QTableView(parent)
{
    initDemo();
}

DemoQTableView::~DemoQTableView()
{

}

void DemoQTableView::initDemo()
{
    auto model = new QemoQFileModel();
    model->initDemo();
    this->setModel(model);
}
//QListView
DemoQListView::DemoQListView(QListView * parent):QListView(parent),m_model(nullptr)
{
    initDemo();
}

DemoQListView::~DemoQListView()
{
    if(m_model)
        delete m_model;
}

void DemoQListView::initDemo()
{
    m_model = new QemoQFileModel();
    m_model->initDemo();
    this->setModel(m_model);
    setRootIndex(m_model->index(QDir::currentPath()));
}
