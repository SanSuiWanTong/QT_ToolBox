#include "centerControl.h"

#include <QApplication>
#include <QScriptEngineDebugger>
#include <QScriptEngine>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDebug>
#include <QDir>
#include <QtGlobal>
#include <iostream>
#include <QMainWindow>
//#include <QJSEngine>
#include <QPushButton>

//Q_DECLARE_METATYPE()

bool attachDebugger(QScriptEngine &engine, QScriptEngineDebugger &debugger)
{
    debugger.attachTo(&engine);
    debugger.setAutoShowStandardWindow(true);
    //QMainWindow *debugWindow = debugger.standardWindow();
    //debugWindow->show();

//    QObject::connect(debugButton, SIGNAL(clicked()),
//                     debugger.action(QScriptEngineDebugger::InterruptAction),
//                     SIGNAL(triggered()));
//    QObject::connect(debugButton, SIGNAL(clicked()),
//                     debugWindow, SLOT(show()));
    return true;
}

QScriptValue evaluateFile(QScriptEngine &engine, const QString &fileName)
{
    QFile scriptFile(fileName);
    if (!scriptFile.open(QIODevice::ReadOnly)){
        qDebug()<<"File open fail:"<<scriptFile.errorString();
        return -1;
    }

    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    scriptFile.close();
    return engine.evaluate(contents);
}

bool loadJS(QScriptEngine &engine,QString& path)
{
    QDir jsDir(path);
    QStringList jsFiles = jsDir.entryList(QStringList("*.js"),QDir::Files);
    foreach (QString file,jsFiles) {
        evaluateFile(engine,jsDir.absoluteFilePath(file));
    }

    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Use QScriptEngine
    //创建QScriptEngine引擎
    QScriptEngine engine;
    //engine.setProcessEventsInterval(100);

    //qScriptRegisterMetaType(engine,QMainWindow);
    //暴露C++给JS
    centerControl* gpControl = new centerControl();
    QScriptValue qsControl = engine.newQObject(gpControl,QScriptEngine::QtOwnership);
    engine.globalObject().setProperty("g_pControl",qsControl);

    //连接调试器
    QScriptEngineDebugger debugger;
    attachDebugger(engine,debugger);

    //读取配置文件
    QString JSPath = "../QT_ToolBox/script/";
    loadJS(engine,JSPath);

    //call JS function,初始化配置
    QScriptValue qsObject = engine.globalObject().property("objGlobal");
    QScriptValueList args;
    args<<"11"<<"22";
    qsObject.property("loadTool").call(qsObject, args);

    //connect to js in C++
    /*//eg:1
    QScriptValue qsTestSlot = qsObject.property("qsTestSlot");
    bool b = qScriptConnect(gpControl->getMainWindow(),SIGNAL(testSignal(int)),qsObject,qsTestSlot);
    */
    /*//eg:2
    QPushButton* bt = new QPushButton();
    QScriptValue qsTestSlot = qsObject.property("qsTestSlot");
    bool b = qScriptConnect(bt,SIGNAL(clicked()),qsObject,qsTestSlot);
    bt->show();
    */
    //qDebug()<<b;

    return a.exec();
}
