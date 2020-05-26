#include "AbstractItemData.h"
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QRandomGenerator>

const QString DATA_FILE = "../QT_ToolBox/data/abstractItemData.txt";
AbstractItemData::AbstractItemData(QObject *parent): QObject(parent)
                                  ,m_bIschecked(false)
{    
    m_parentItem = static_cast<AbstractItemData*>(parent);
    //if(!QFile::exists(DATA_FILE))
        generateDemoData();
        //generateDemoDataForCensus();
}

AbstractItemData::~AbstractItemData()
{
    qDeleteAll(m_dataChildList.begin(), m_dataChildList.end());
    m_dataChildList.clear();
}
void AbstractItemData::generateDemoDataForCensus()
{
    QFile data(DATA_FILE);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&data);
        QRandomGenerator gen;
        for(int i = 0 ; i < 10 ; i++){
                int year    = gen.bounded(1000, 2000);
                int Males   = gen.bounded(10000, 30000);
                int Females = gen.bounded(10000, 30000);
                out<<year<<" "<<Males<<" "<<Females<<" "<<Males+Females<<"\r\n";
        }
    }
}
void AbstractItemData::generateDemoData()
{
    QFile data(DATA_FILE);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&data);
        const int ROW_MAX    = 20;
        const int ROW_MIN    = 3;
        const int COLUMN_MAX = 1;

        for(int i = 10 ; i < ROW_MAX ; i++){
            for(int col_i = 0 ; col_i < COLUMN_MAX ; col_i++){
                out<<"Item:"<<i<<","<<col_i<<" ";
            }
            out<<"\r\n";

            for(int j = 0; j < ROW_MAX ; j++){
                for(int col_j = 0 ; col_j < COLUMN_MAX ; col_j++){
                    out<<"Item:"<<i<<"_"<<j<<","<<col_j<<" ";
                }
                out<<"\r\n";

                for(int k = 0 ; k < ROW_MIN ; k++){
                    for(int col_k = 0 ; col_k < COLUMN_MAX ; col_k++){
                        out<<"Item:"<<i<<"_"<<j<<"_"<<k<<","<<col_k<<" "; //qSetFieldWidth(8)
                    }
                    out<<"\r\n";
                }
            }
        }
    } else {
        qDebug()<<"file open fail:"<<data.errorString();
    }
    data.close();
}

void AbstractItemData::load()
{   
    QFile data(DATA_FILE);
    if(data.open(QFile::ReadOnly)){
         QTextStream in(&data);
         QString line;
         AbstractItemData* p_leval = this;
         while (in.readLineInto(&line)) {            
            parseStr(&p_leval,line);
         }
    } else {
        qDebug()<<"file open fail:"<<data.errorString();
    }
    data.close();
    qDebug()<<"load finish:";
}

/* p
 * ->text
 * ->childList<p>
 */
void AbstractItemData::parseStr(AbstractItemData **p_leval, QString& str,int row)
{
    QString     strData = str.trimmed();
    QStringList strList = strData.split(" ");

    DataType t_data;
    if(!(*p_leval)->m_dataColList.isEmpty())
        t_data = (*p_leval)->m_dataColList.at(0);

    if(t_data.dataText.length() < strList.at(0).length()){
        auto childItem = new AbstractItemData(*p_leval);        
        foreach(auto s,strList){
            childItem->m_dataColList<<(DataType){s,"",""};
        }
        if(row >= 0)
            (*p_leval)->m_dataChildList.insert(row,childItem);
        else
            (*p_leval)->m_dataChildList<<childItem;

        *p_leval = childItem;               //use *p_level change level
    }else{
        *p_leval = (*p_leval)->m_parentItem;
        parseStr(p_leval,str);
    }
}

int AbstractItemData::childCount() const
{
    return this->m_dataChildList.length();
}

int AbstractItemData::columnCount() const
{
    if(!this->m_dataChildList.isEmpty())
        return this->m_dataChildList.at(0)->m_dataColList.length();
    return 0;
}

int AbstractItemData::rowOfChild() const
{
    if(m_parentItem)
        return m_parentItem->m_dataChildList.indexOf(const_cast<AbstractItemData*>(this));
    return 0;
}

QVariant AbstractItemData::getData(int col) const
{
    if(col >= this->m_dataColList.length())
        return QVariant();

    return this->m_dataColList.at(col).dataText;
}

AbstractItemData* AbstractItemData::childAt(int row) const
{
    if(row >= this->m_dataChildList.length())
        return nullptr;

    return this->m_dataChildList.at(row);
}

AbstractItemData* AbstractItemData::parent()         const
{
    return this->m_parentItem;
}
void     AbstractItemData::addChild(AbstractItemData* item)
{
    this->m_dataChildList<<item;
}

bool    AbstractItemData::getChecked()
{
    return m_bIschecked;
}

void    AbstractItemData::setChecked(bool c)
{
    foreach(auto it,this->m_dataChildList)
        it->setChecked(c);
    m_bIschecked = c;
}

bool    AbstractItemData::setData(int col, const QVariant &var)
{
    m_dataColList.replace(col,(DataType){var.toString(),"",""});
    return true;
}

bool    AbstractItemData::removeChild(int i)
{
    auto t =  this->m_dataChildList.at(i);
    this->m_dataChildList.removeAt(i);
    delete t;
    return true;
}

void    AbstractItemData::testItemChange()
{
    AbstractItemData* t = m_dataChildList.at(2)->m_dataChildList.at(3);
    QString str = "test change";
    t->setData(0,"test Change");
    emit itemDataChanged(t);
}

void  AbstractItemData::getItemAndChildStream(QTextStream& stream, AbstractItemData* item)
{
    if(0 == item)                           item = this;
    foreach(auto c,item->m_dataColList)     stream<<c.dataText<<"\r\n";
    foreach(auto c,item->m_dataChildList)   getItemAndChildStream(stream,c);
    stream.flush();
}

void  AbstractItemData::setItemAndChildStream(QTextStream* stream, int row, int col, AbstractItemData* item)
{
    Q_UNUSED(col)
    if(0 == item)
        item = this;

    AbstractItemData* p_leval = (row >= 0) ? item->parent() : item;
    QString line;
    while(stream->readLineInto(&line)){
       parseStr(&p_leval,line,row);
    }
}
