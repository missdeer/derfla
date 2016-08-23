#include "stdafx.h"
#include "candidatelistwidget.h"

CandidateListWidget::CandidateListWidget(QWidget *parent)
    : QListWidget(parent)
{

}

void CandidateListWidget::keyPressEvent(QKeyEvent *event)
{
    check_expiration;
    QListWidget::keyPressEvent(event);
    //qDebug() << "CandidateListWidget::keyPressEvent" << event;
    if (event->text().length() > 0)
    {
        emit keyPressedEvent(event);
    }
}
