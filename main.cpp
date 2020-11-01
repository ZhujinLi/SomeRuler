#include "someruler.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);

    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    SomeRuler ruler;
    ruler.setWindowIcon(QIcon(":/images/app.png"));
    ruler.show();

    return a.exec();
}
