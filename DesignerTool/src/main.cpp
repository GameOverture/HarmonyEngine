#include "MainWindow.h"
#include <QApplication>

#include "HyGlobal.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    HyGlobal::Initialize();

    MainWindow w;
    w.show();

    return a.exec();
}
