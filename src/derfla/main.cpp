#include "derflawidget.h"
#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DerflaWidget w;
    w.show();

    QScreen* scr = a.primaryScreen();
    QSize sz = scr->availableSize();
    w.move(sz.width()/4, sz.height()/5);

    return a.exec();
}
