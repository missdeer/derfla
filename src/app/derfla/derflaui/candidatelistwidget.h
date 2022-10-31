#ifndef CANDIDATELISTWIDGET_H
#define CANDIDATELISTWIDGET_H

#include <QListWidget>

class CandidateListWidget : public QListWidget
{
    Q_OBJECT
public:
    CandidateListWidget(QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent *event);
signals:
    void keyPressedEvent(QKeyEvent *);
};

#endif // CANDIDATELISTWIDGET_H
