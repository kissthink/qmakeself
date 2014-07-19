#include "qmakeself.h"
#include <QApplication>
#include <QStyleFactory>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMakeSelf w;
    a.setStyle(QStyleFactory::create("Fusion"));
    w.show();

    return a.exec();
}
