#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setWindowIcon(QIcon(":/Images/Images/happiness.png"));
    MainWindow w;
    w.show();
    return a.exec();
}


