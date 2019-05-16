#ifndef CANDIDATELIST_H
#define CANDIDATELIST_H

#include <QFrame>
#include <QIcon>
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
    explicit CandidateList(QWidget *parent = nullptr);
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

public slots:

private slots:
    void onActionUpdated();
    void onEmptyAction();
private:
    Ui::CandidateList *ui;
    bool activeWindowFlag_;
    bool cleared_;
    QMap<QString, QIcon> actionIconMap_;
    void clearData();
};

#endif // CANDIDATELIST_H
