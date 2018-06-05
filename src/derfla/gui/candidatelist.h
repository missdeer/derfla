#ifndef CANDIDATELIST_H
#define CANDIDATELIST_H

#include <QFrame>
#include "candidatelistwidget.h"
#include "derflaaction.h"

namespace Ui {
class CandidateList;
}
class ExtensionManager;

class CandidateList : public QFrame
{
    Q_OBJECT

public:
    explicit CandidateList(ExtensionManager* extensionManager, QWidget *parent = 0);
    ~CandidateList();
    void update(const QString& text);

    void populateList();

    void refreshList();

    int count() const;
    bool getActiveWindowFlag() const;
    void setActiveWindowFlag(bool value);

    void onEnter();
protected:
    void keyPressEvent(QKeyEvent* event);
    void showEvent(QShowEvent *event);
signals:
    void done();
    void keyPressedEvent(QKeyEvent* event);
private slots:
    void actionUpdated(DerflaActionList& dal);
private:
    Ui::CandidateList *ui;
    bool activeWindowFlag_;
    int itemCount_ ;
    ExtensionManager* extensionManager_;
    DerflaActionList dal_;
};

#endif // CANDIDATELIST_H
