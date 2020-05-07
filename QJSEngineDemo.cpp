#include "QJSEngineDemo.h"
#include "centerControl.h"
#include <QDebug>
#include <QDir>

QJSEngineDemo::QJSEngineDemo() : m_pJSEngin(new QJSEngine())
{
}

void QJSEngineDemo::initCenterControl()
{
    QJSValue jsMetaObject = m_pJSEngin->newQMetaObject(&centerControl::staticMetaObject);
    m_pJSEngin->globalObject().setProperty("CenterControl", jsMetaObject);
}

void QJSEngineDemo::loadJSFiles()
{
    const QString JSPath = "../QT_ToolBox/script/";
    QDir jsDir(JSPath);
    QStringList jsFiles = jsDir.entryList(QStringList("*.js"),QDir::Files);

    foreach (QString file,jsFiles) {
        QString fileName = jsDir.absoluteFilePath(file);
        QFile scriptFile(fileName);

        if (!scriptFile.open(QIODevice::ReadOnly)){
            qDebug()<<"File open fail:"<<scriptFile.errorString();
            continue;
        }
        QTextStream stream(&scriptFile);
        QString contents = stream.readAll();
        scriptFile.close();

        QJSValue result = m_pJSEngin->evaluate(contents,fileName);
        printException(result);
    }
}

void QJSEngineDemo::exeJsFun(QString& fun)
{
    QJSValue jsObj = m_pJSEngin->globalObject().property("objGlobal");
    QJSValue result = jsObj.property(fun).call(QJSValueList()<<"11"<<"22");
    printException(result);
}

void QJSEngineDemo::printException(QJSValue& result)
{
    if (result.isError())
      qDebug()<< "Uncaught exception at line"
              << result.property("lineNumber").toInt()
              << ":" << result.toString();
}
