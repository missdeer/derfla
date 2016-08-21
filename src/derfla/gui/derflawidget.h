#ifndef DerflaWidget_H
#define DerflaWidget_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QPixmap>
#include <QTimer>
#include <QSystemTrayIcon>

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
private slots:
    void inputChanged(const QString& text);
    void keyPressed(QKeyEvent* e);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void loadSkin();
    void onTimer();
    void showInFront();
    void candidateListDone();
private:
    QThread scanThread;
    QPoint mouseMovePos;
    QPixmap backgroundImage;
    QTimer* timer;
    CharLineEdit* input;
    QSystemTrayIcon* trayicon;
    CandidateList* candidatelist;
    UGlobalHotkeys *hotkeyManager;
    LocalFSScanner* localFSScanner;
    void ShowCandidateList();
    void processKey();
    void doEnter();
    void doTab();
    void doBackTab();
    bool applySkin(const QString& skin);
    void waiting();
    void stopWaiting();
    void HideCandidateList();
};

#endif // DerflaWidget_H
