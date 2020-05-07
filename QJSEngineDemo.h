#ifndef QJSENGINEDEMO_H
#define QJSENGINEDEMO_H
#include <QJSEngine>

class QJSEngineDemo
{
public:
    QJSEngineDemo();
    void initCenterControl();
    void loadJSFiles();
    void exeJsFun(QString& fun);

private:
    void printException(QJSValue& result);
    QJSEngine* m_pJSEngin;
};

#endif // QJSENGINEDEMO_H
