#ifndef HOTKEY_H
#define HOTKEY_H

#include <QKeyEvent>
#include <QLineEdit>
#include <QString>
#include <QWidget>

#include "setting.h"

namespace Ui
{
    class hotkey;
}

class hotkey : public QWidget
{
    Q_OBJECT

public:
    explicit hotkey(QWidget *parent = 0);
    ~hotkey();
    void       keyPressEvent(QKeyEvent *event);
    void       keyReleaseEvent(QKeyEvent *event);
    QLineEdit *s;

private:
    Ui::hotkey *ui;
};

#endif // HOTKEY_H
