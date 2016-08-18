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
    int count() const;
    bool getActiveWindowFlag() const;
    void setActiveWindowFlag(bool value);

    void onEnter();
protected:
    void keyPressEvent(QKeyEvent* event);
    void showEvent(QShowEvent *event);
signals:
    void done();
private slots:
    void on_listWidget_pressed(const QModelIndex &index);

private:
    Ui::CandidateList *ui;
    bool activeWindowFlag;
};

#endif // CANDIDATELIST_H
