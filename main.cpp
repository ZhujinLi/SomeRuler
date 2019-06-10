#include "qkruler.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    QkRuler w;
    w.show();

    return a.exec();
}
