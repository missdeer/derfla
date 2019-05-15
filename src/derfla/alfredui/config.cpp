#include "config.h"
#include <QScreen>

void CenterToScreen(QWidget* widget)
{
    if (!widget)
        return;
    QScreen* scr = qApp->primaryScreen();
    QSize sz = scr->availableSize();
    int desk_x = sz.width();
    int desk_y = sz.height();
    int x = widget->width();
    widget->move(desk_x / 2 - x / 2, 
                (desk_y - beginheight - rowsize * (MAXPRINTSIZE - 1)) * widget->height());
}
