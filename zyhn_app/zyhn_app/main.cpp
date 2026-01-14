#include "zyhn_app.h"
#include "exception_handler.h"
#include <QtWidgets/QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    ExceptionHandler::initialize();
    
    QApplication app(argc, argv);
    QFile file(":/zyhn_app/style/qss/common.qss");
    if (file.open(QFile::ReadOnly)) {
        app.setStyleSheet(QString::fromUtf8(file.readAll()));
        file.close();
    } 
    
    ZyhnApp *zyhn_app = new ZyhnApp();
    zyhn_app->load();

    int ret = app.exec();
    delete zyhn_app;
    return ret;
}
