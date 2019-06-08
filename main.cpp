#include "qkruler.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QkRuler w;
    w.show();

    return a.exec();
}
