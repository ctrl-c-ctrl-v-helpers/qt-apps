#include "menustop.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MenuStop w;
    w.show();
    return a.exec();
}
