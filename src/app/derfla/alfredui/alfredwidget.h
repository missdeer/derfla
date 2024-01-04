#ifndef WIDGET_H
#define WIDGET_H

#include <map>
#include <string>
#include <vector>

#include <QLabel>
#include <QListWidget>
#include <QMutex>
#include <QWidget>

#include "commonwidget.h"
#include "plaintext.h"
#include "thememanager.h"

namespace Ui
{
    class Widget;
}

class AlfredWidget : public CommonWidget
{
    Q_OBJECT
public:
    explicit AlfredWidget(QWidget *parent = nullptr);
    ~AlfredWidget() override;
    AlfredWidget(const AlfredWidget &)             = delete;
    AlfredWidget &operator=(const AlfredWidget &)  = delete;
    AlfredWidget(const AlfredWidget &&)            = delete;
    AlfredWidget &operator=(const AlfredWidget &&) = delete;
    void          onTextChanged();
    void          setOne();
    void          enterCurItem();
    void          populateList();
signals:
    void done();
private slots:
    void onActionUpdated();
    void onEmptyAction();
    void onEnterItem(int index);
    void onEscape();
    void onDone();
    void onCustomContextMenuRequested(const QPoint &pos);

private:
    Ui::Widget  *ui;
    PlainText   *m_plainTextEdit;
    QListWidget *m_listWidget;
    AlfredTheme *m_theme;
    ThemeManager m_themeManager;
    void         setUpTheme();
    void         hideCandidateList() override;

    void onLoadSkin() override;
    void onStayOnTop() override;
    void onSelectFile() override;
    void onSelectFolder() override;
    bool onPreference() override;
};
#endif // WIDGET_H
