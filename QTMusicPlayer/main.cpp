#include "widget.h"
#include "loadfileqss.h"
#include <QApplication>
#include <dbManager.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoadFileQss::setStyle("./Allthe.qss"); //加载qss 样式表，用静态方式
    dbManager::destoryAllDBConn();
    Widget w;
    w.show();
    return a.exec();
}
