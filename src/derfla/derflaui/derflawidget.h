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
class QGlobalShortcut;
class SkinManager;

class DerflaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DerflaWidget(ExtensionManager *em, QWidget *parent = nullptr);
    ~DerflaWidget() override;
protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusOutEvent(QFocusEvent*) override;
    void focusInEvent(QFocusEvent *e) override;
signals:

private slots:
    void onInputChanged(const QString& text);
    void onKeyPressed(QKeyEvent* e);
    void onFocusOut(QFocusEvent *);
    void onFocusIn(QFocusEvent *);
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
    void onCheckUpdates();
    void onCustomContextMenuRequested(const QPoint &pos);
private:
    const int widgetMinWidth_ = 450;
    QPoint mouseMovePos_;

    QTimer* candidateDelayTimer_;
    CharLineEdit* input_;
    QSystemTrayIcon* trayIcon_;
    ExtensionManager* extensionManager_;
    CandidateList* candidateList_;
#if defined (Q_OS_WIN)
    QGlobalShortcut *hotkeyManager_;
#else
    UGlobalHotkeys *hotkeyManager_;
#endif
    SkinManager* skinManager_;
    QAction *stayOnTopAction_;
    int candidateDelayInterval_;
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
