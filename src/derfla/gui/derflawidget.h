#ifndef DerflaWidget_H
#define DerflaWidget_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QPixmap>
#include <QTimer>
#include <QSystemTrayIcon>
#include "alfredworkflow.h"

class CharLineEdit;
class CandidateList;
class UGlobalHotkeys;
class LocalFSScanner;

class DerflaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DerflaWidget(QWidget *parent = 0);
    ~DerflaWidget();
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);    
    void moveEvent(QMoveEvent *event);

    void keyPressEvent(QKeyEvent* event);
signals:

private slots:
    void inputChanged(const QString& text);
    void keyPressed(QKeyEvent* e);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void loadSkin();
    void onLoadingAnimationTimer();
    void showInFront();
    void candidateListDone();
    void quit();
    void installAlfredWorkflows();
    void scheduleScan();
    void finishedScan();
    void loadInstalledAlfredWorkflows();
private:
    const int widgetMinWidth_ = 450;
    QPoint mouseMovePos_;
    QPixmap backgroundImage_;
    QPixmap left;
    QPixmap mid;
    QPixmap right;
    QTimer* loadingAnimationTimer_;
    CharLineEdit* input_;
    QSystemTrayIcon* trayIcon_;
    CandidateList* candidateList_;
    UGlobalHotkeys *hotkeyManager_;
    LocalFSScanner* localFSScanner_;
    AlfredWorkflowList alfredWorkflowList_;
    void showCandidateList();
    void processKey();
    void doEnter();
    void doTab();
    void doBackTab();
    bool applySkin(const QString& skin);
    void waiting();
    void stopWaiting();
    void hideCandidateList();
    bool loadSkinConfiguration(const QString& configurationPath, QString& bgImagePath, QString& inputStyle);
    bool loadSkinPackage(const QString& skinPath, QString& configurationPath);
};

#endif // DerflaWidget_H
