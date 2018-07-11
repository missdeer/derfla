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
    void onCustomContextMenuRequested(const QPoint &pos);
private:
    const int widgetMinWidth_ = 450;
    QPoint mouseMovePos_;
    QPixmap backgroundImage_;
    QPixmap leftPartBackgroundImage_;
    QPixmap midPartBackgroundImage_;
    QPixmap rightPartBackgroundImage_;
    QTimer* loadingAnimationTimer_;
    QTimer* candidateDelayTimer_;
    CharLineEdit* input_;
    QSystemTrayIcon* trayIcon_;
    ExtensionManager* extensionManager_;
    CandidateList* candidateList_;
    UGlobalHotkeys *hotkeyManager_;
    bool stayOnTop_;
    void processKey();
    void doEnter();
    void doTab();
    void doBackTab();
    bool applySkin(const QString& skin);
    void stopWaiting();
    void showCandidateList();
    void hideCandidateList();
    bool loadSkinConfiguration(const QString& configurationPath, QString& bgImagePath, QString& inputStyle, int& cutTop, int& cutBottom);
    bool loadSkinPackage(const QString& skinPath, QString& configurationPath);
};

#endif // DerflaWidget_H
