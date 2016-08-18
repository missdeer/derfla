#ifndef DerflaWidget_H
#define DerflaWidget_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QPixmap>
#include <QTimer>

class CharLineEdit;

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
private slots:
    void focusInEvent(QFocusEvent* event);
    void focusOutEvent(QFocusEvent* event);
    void inputMethodEvent(QInputMethodEvent* event);
    void inputKeyPressEvent(QKeyEvent* event);
private:
    QPoint mouseMovePos;
    QPixmap pic;
    QTimer* timer;
    CharLineEdit* input;
    void processKey();
};

#endif // DerflaWidget_H
