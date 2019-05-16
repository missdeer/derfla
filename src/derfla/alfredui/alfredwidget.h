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
#include "alfredtheme.h"
#include "commonwidget.h"

using pair = std::vector<std::pair<std::string, std::pair<std::string, QIcon*>>> ;
using qpair = std::pair<std::string, std::pair<std::string, QIcon*>> ;
namespace Ui {
class Widget;
}

class AlfredWidget : public CommonWidget
{
    Q_OBJECT
public:
    explicit AlfredWidget(QWidget *parent = nullptr);
    ~AlfredWidget() override;
    void onTextChanged();
    void setOne();
    void enterCurItem();
    void populateList();
private slots:
    void onActionUpdated();
    void onEmptyAction();
private:
    Ui::Widget *ui;
    QMutex* mutex_thread;
    pair map;
    PlainText *plainTextEdit;
    QListWidget *listWidget;
    AlfredTheme* theme;
    void setUpTheme();
};
#endif // WIDGET_H
