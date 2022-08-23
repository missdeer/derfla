#ifndef DerflaWidget_H
#define DerflaWidget_H

#include <QMouseEvent>
#include <QPixmap>
#include <QPoint>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWidget>

#include "commonwidget.h"

class CharLineEdit;
class CandidateList;
class SkinManager;

class DerflaWidget : public CommonWidget
{
    Q_OBJECT

public:
    explicit DerflaWidget(QWidget *parent = nullptr);
    ~DerflaWidget() override;

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *) override;
    void focusInEvent(QFocusEvent *e) override;
signals:

private slots:
    void onInputChanged(const QString &text);
    void onInputFocusOut(QFocusEvent *);
    void onInputFocusIn(QFocusEvent *);
    void onCandidateListKeyPressed(QKeyEvent *e);
    void onCandidateListDone();
    void onCandidateDelayTimer();
    void onCustomContextMenuRequested(const QPoint &pos);

private:
    const int widgetMinWidth_ = 450;
    QPoint    mouseMovePos_;

    QTimer        *candidateDelayTimer_;
    CharLineEdit  *input_;
    CandidateList *candidateList_;
    SkinManager   *skinManager_;
    int            candidateDelayInterval_;

    void processKey();
    void doEnter();
    void doTab();
    void doBackTab();
    bool applySkin(const QString &skin);
    void stopWaiting();
    void showCandidateList();

    void hideCandidateList() override;
    void onLoadSkin() override;
    void onStayOnTop() override;
    void onSelectFile() override;
    void onSelectFolder() override;
    bool onPreference() override;
};

#endif // DerflaWidget_H
