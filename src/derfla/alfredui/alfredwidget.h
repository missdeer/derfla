#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <vector>
#include <QListWidget>
#include <QMutex>
#include <string>
#include <map>
#include "plaintext.h"
#include "alfredtheme.h"
#include "commonwidget.h"

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
    PlainText *plainTextEdit;
    QListWidget *listWidget;
    AlfredTheme* theme;
    void setUpTheme();
};
#endif // WIDGET_H
