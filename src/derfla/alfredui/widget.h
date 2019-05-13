#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <vector>
#include <QListWidget>
#include <QMutex>
#include <string>
#include <map>
#include "widgetthread.h"
#include "plaintext.h"
#include "returnbyscript.h"
#include "config.h"
#include "singleapplication.h"
#include "theme.h"

using pair = std::vector<std::pair<std::string, std::pair<std::string, QIcon*>>> ;
using qpair = std::pair<std::string, std::pair<std::string, QIcon*>> ;
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    friend class WidgetThread;
public slots:
    void paint_mutex();
    void OtherhotkeyPressed(std::string, bool);
public:
    explicit Widget(QWidget *parent = nullptr);
    void defaultsearch();
    void searchApp();
    std::vector<returnByScript> val;
    void hotkeyPressed();
    void setOne();
    void enterCurItem();
    float Height;
    ~Widget() override;

private:
    void paint();
    Ui::Widget *ui;
    QMutex* mutex_paint;
    QMutex* mutex_thread;
    pair map;
    PlainText *plainTextEdit;
    QListWidget *listWidget;
    Theme* theme;
    void setUpTheme();
};
#endif // WIDGET_H