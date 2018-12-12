#ifndef CANDIDATELIST_H
#define CANDIDATELIST_H

#include <QFrame>
#include "candidatelistwidget.h"
#include "actionexecutor.h"

namespace Ui {
class CandidateList;
}
class ExtensionManager;
class ActionExecutor;

class CandidateList : public QFrame
{
    Q_OBJECT

public:
    explicit CandidateList(ExtensionManager* extensionManager, QWidget *parent = nullptr);
    ~CandidateList();
    void update(const QString& text);

    void populateList();
    void refreshList();
    int count() const;
    bool getActiveWindowFlag() const;
    void setActiveWindowFlag(bool value);
    void clear();
    void onEnter();
    void setInputBoxSize(const QSize& size);
protected:
    void keyPressEvent(QKeyEvent* event);
    void showEvent(QShowEvent *event);
signals:
    void done();
    void keyPressedEvent(QKeyEvent* event);
    void viaPaypal();
    void viaAlipay();
    void viaWeChatPay();
private slots:
    void onActionUpdated(DerflaActionList& dal);
    void onEmptyAction();
private:
    Ui::CandidateList *ui;
    bool activeWindowFlag_;
    bool cleared_;
    int itemCount_ ;
    ExtensionManager* extensionManager_;
    ActionExecutor actionExecutor_;
    DerflaActionList dal_;
    DerflaActionList dalDonate_;
    void createDonateDerflaActions();
};

#endif // CANDIDATELIST_H
