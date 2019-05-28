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
#include "commonwidget.h"
#include "thememanager.h"

namespace Ui {
class Widget;
}

class AlfredWidget : public CommonWidget
{
    Q_OBJECT
public:
    explicit AlfredWidget(QWidget *parent = nullptr);
    ~AlfredWidget() override;
    AlfredWidget(const AlfredWidget&) = delete;
    AlfredWidget& operator=(const AlfredWidget&) = delete;
    AlfredWidget(const AlfredWidget&&) = delete;
    AlfredWidget& operator=(const AlfredWidget&&) = delete;
    void onTextChanged();
    void setOne();
    void enterCurItem();
    void populateList();
signals:
    void done();
private slots:
    void onActionUpdated();
    void onEmptyAction();
    void onEnterItem(int index);
    void onEscape();
    void onDone();
private:
    Ui::Widget *ui;
    PlainText *plainTextEdit;
    QListWidget *listWidget;
    AlfredTheme* theme;
    ThemeManager themeManager;
    void setUpTheme();
    void hideCandidateList() override;

    void onLoadSkin() override;
    void onStayOnTop() override;
    void onSelectFile() override;
    void onSelectFolder() override;
    bool onPreference() override;
};
#endif // WIDGET_H
