#include <QApplication>
#include "serverdisk.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerDisk serverGo;
    return a.exec();
}
