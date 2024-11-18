#include "clearformatapp.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClearFormatApp w;
    w.show();
    return a.exec();
}
