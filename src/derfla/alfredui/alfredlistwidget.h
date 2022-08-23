#ifndef LISTWIDGET_H
#define LISTWIDGET_H
#include <QListWidget>
#include <QWidget>

#include "plaintext.h"

class AlfredListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit AlfredListWidget(QWidget *parent = 0);
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // LISTWIDGET_H
