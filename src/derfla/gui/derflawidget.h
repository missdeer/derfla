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
    void inputChanged(const QString& text);
    void keyPressed(QKeyEvent* e);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void loadSkin();
	void installExtension();
    void onLoadingAnimationTimer();
    void showInFront();
    void candidateListDone();
    void quit();
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
    void processKey();
    void doEnter();
    void doTab();
    void doBackTab();
    bool applySkin(const QString& skin);
    void waiting();
    void stopWaiting();
    void showCandidateList();
    void hideCandidateList();
    bool loadSkinConfiguration(const QString& configurationPath, QString& bgImagePath, QString& inputStyle, int& cutTop, int& cutBottom);
    bool loadSkinPackage(const QString& skinPath, QString& configurationPath);
};

#endif // DerflaWidget_H
