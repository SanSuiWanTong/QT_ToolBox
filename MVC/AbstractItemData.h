#ifndef DATAITEM_H
#define DATAITEM_H
#include <QList>
#include <QVariant>
#include <QTextStream>

class AbstractItemData;

struct DataStruct{
    QString dataText;
    QString dataIcon;
    QString other;
};

typedef  DataStruct               DataType;
typedef  QList<DataType>          DataColList;
typedef  QList<AbstractItemData*> DataItemList;

class AbstractItemData : public QObject
{
    Q_OBJECT
public:
    explicit AbstractItemData(QObject *parent=0);
    ~AbstractItemData();

    void     load();    
    void     setChecked(bool);
    void     addChild(AbstractItemData*,int pos=-1);
    void     getItemAndChildStream(QTextStream &stream, AbstractItemData*item = 0);
    void     setItemAndChildStream(QTextStream *stream, int row, int col, AbstractItemData*item = 0);

    int      childCount()       const;
    int      columnCount()      const;
    int      rowOfChild()       const;

    QVariant getData(int col)   const;

    AbstractItemData* parent()         const;
    AbstractItemData* childAt(int row) const;    

    bool    getChecked();
    bool    setData(int col,const QVariant&);
    bool    removeChild(int);
    //test
    void     testItemChange();
    void     testItemAdd(int row);
    //
signals:
    void    itemDataChanged(AbstractItemData*);

private:
    void     generateDemoData();
    void     generateDemoDataForCensus();
    void     parseStr(AbstractItemData **, QString&,int row = -1);

private:    
    DataColList         m_dataColList;
    DataItemList        m_dataChildList;
    AbstractItemData*   m_parentItem;  

    bool    m_bIschecked;
};



#endif // DATAITEM_H
