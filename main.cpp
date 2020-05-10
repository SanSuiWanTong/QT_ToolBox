#include <QApplication>
#include <QSplashScreen>
#include "QJSEngineDemo.h"

void preLoadScreen(QWidget * win,QApplication& a){
    QPixmap pic("../QT_ToolBox/images/splash.jpg");
    QSplashScreen *splash = new QSplashScreen(pic.scaled(800,600));
    splash->show();
    splash->showMessage(QObject::tr("Loading ... "),Qt::AlignRight|Qt::AlignTop,Qt::white);
    a.processEvents();
    splash->finish(win);
    delete splash;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //preLoadScreen(NULL,a);

    QJSEngineDemo jsEngine;
    jsEngine.initCenterControl();
    jsEngine.loadJSFiles();
    QString fun = "loadTools";
    jsEngine.exeJsFun(fun);

    return a.exec();
}
