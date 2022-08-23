#ifndef GETICON_H
#define GETICON_H

#include <QIcon>
#include <QPixmap>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtCore/QtDebug>
#include <QtGui/QImage>
#undef signals

extern "C"
{
#include <gdk/gdk.h>
}

QImage toQImage(GdkPixbuf *pixbuf);
QIcon  toQIcon(std::string icon);
QIcon  toQIcon(GdkPixbuf *pixbuf);

#endif // GETICON_H
