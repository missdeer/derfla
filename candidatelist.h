#ifndef CANDIDATELIST_H
#define CANDIDATELIST_H

#include <QFrame>

namespace Ui {
class CandidateList;
}

class CandidateList : public QFrame
{
    Q_OBJECT

public:
    explicit CandidateList(QWidget *parent = 0);
    ~CandidateList();
    void update(const QString& text);
private slots:
    void on_listWidget_pressed(const QModelIndex &index);

private:
    Ui::CandidateList *ui;
};

#endif // CANDIDATELIST_H
