#ifndef DerflaWidget_H
#define DerflaWidget_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QPixmap>
#include <QTimer>
#include <QSystemTrayIcon>

class ExtensionManager;
class CharLineEdit;
class CandidateList;
class UGlobalHotkeys;
class AutoUpdater;
class SkinManager;

class DerflaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DerflaWidget(QWidget *parent = nullptr);
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
    void onInputChanged(const QString& text);
    void onKeyPressed(QKeyEvent* e);
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onLoadSkin();
	void onInstallExtension();
    void onStayOnTop();
    void onShowInFront();
    void onCandidateListDone();
    void onQuit();
    void onCandidateDelayTimer();
    void onSelectFile();
    void onSelectFolder();
    void onAbout();
    void onPreference();
    void onCustomContextMenuRequested(const QPoint &pos);
private:
    const int widgetMinWidth_ = 450;
    QPoint mouseMovePos_;

    QTimer* candidateDelayTimer_;
    CharLineEdit* input_;
    QSystemTrayIcon* trayIcon_;
    ExtensionManager* extensionManager_;
    CandidateList* candidateList_;
    UGlobalHotkeys *hotkeyManager_;
    AutoUpdater* autoUpdater_;
    SkinManager* skinManager_;
    bool stayOnTop_;
    void processKey();
    void doEnter();
    void doTab();
    void doBackTab();
    bool applySkin(const QString& skin);
    void stopWaiting();
    void showCandidateList();
    void hideCandidateList();
};

#endif // DerflaWidget_H
