#include <iostream>

#include <QMouseEvent>

#include "alfredlistwidget.h"

AlfredListWidget::AlfredListWidget(QWidget *parent) : QListWidget(parent)
{
    //    this->setMouseTracking(true);
}

void AlfredListWidget::mouseMoveEvent(QMouseEvent *event)
{
    //    int row = indexAt(event->pos()).row();
    //    this->setCurrentRow(row);
}
