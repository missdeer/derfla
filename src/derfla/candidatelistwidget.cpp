#include <QtCore>
#include <QKeyEvent>
#include "candidatelistwidget.h"

CandidateListWidget::CandidateListWidget(QWidget *parent)
    : QListWidget(parent)
{

}

void CandidateListWidget::keyPressEvent(QKeyEvent *event)
{
    QListWidget::keyPressEvent(event);
    qDebug() << __FUNCTION__ << event;
    if (event->text().length() > 0)
    {
        emit keyPressedEvent(event);
    }
}
