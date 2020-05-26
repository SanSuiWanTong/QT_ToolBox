#ifndef QTREETABLELISTWIDGETDEMO_H
#define QTREETABLELISTWIDGETDEMO_H
#include <QTreeWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QTreeView>
#include <QTableView>
#include <QListView>
#include <QFileSystemModel>

class DemoQTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit DemoQTreeWidget(QWidget* parent = 0);
    virtual ~DemoQTreeWidget();

    void initDemo();
};

class DemoQTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit DemoQTableWidget(QWidget* parent = 0);
    virtual ~DemoQTableWidget();

    void initDemo();
};

class DemoQListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit DemoQListWidget(QWidget* parent = 0);
    virtual ~DemoQListWidget();

    void initDemo();
};

class QemoQFileModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit QemoQFileModel(QFileSystemModel* parent = 0);
    virtual ~QemoQFileModel();

    void initDemo();
};

class DemoQTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit DemoQTreeView(QTreeView* parent = 0);
    virtual ~DemoQTreeView();

    void initDemo();
};
class DemoQTableView : public QTableView
{
    Q_OBJECT
public:
    explicit DemoQTableView(QTableView* parent = 0);
    virtual ~DemoQTableView();

    void initDemo();
};
class DemoQListView : public QListView
{
    Q_OBJECT
public:
    explicit DemoQListView(QListView* parent = 0);
    virtual ~DemoQListView();

    void initDemo();
private:
    QemoQFileModel* m_model;
};

#endif // QTREETABLELISTWIDGETDEMO_H
