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
private:
    QPoint mouseMovePos;
    QPixmap pic;
    QTimer* timer;
    CharLineEdit* input;
    QSystemTrayIcon* trayicon;
    CandidateList* candidatelist;
    void ShowCandidateList();
    void processKey();
    void doEnter();
    void doTab();
    void doBackTab();
};

#endif // DerflaWidget_H
