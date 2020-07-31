#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // DPI support
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps); //HiDPI pixmaps
    QApplication a(argc, argv);
    a.setFont(QFont("Tahoma", 10, QFont::Normal), "QWidget");
    a.setFont(QFont("Consolas", 10, QFont::Normal), "QTextEdit");
    MainWindow w;
    return a.exec();
}
