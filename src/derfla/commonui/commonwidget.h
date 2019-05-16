#ifndef COMMONWIDGET_H
#define COMMONWIDGET_H

#include <QWidget>
#if defined(Q_OS_WIN)
#include "qglobalshortcut.h"
#else
#include "uglobalhotkeys.h"
#endif

class CommonWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CommonWidget(QWidget *parent = nullptr);
    
signals:
    
public slots:
    virtual void onLoadSkin();
    virtual void onStayOnTop();
    virtual void onShowInFront();
    virtual void onSelectFile();
    virtual void onSelectFolder();
    virtual void onPreference();
    
protected:
#if defined (Q_OS_WIN)
    QGlobalShortcut *hotkeyManager_;
#else
    UGlobalHotkeys *hotkeyManager_;
#endif
    bool stayOnTop_;
};

#endif // COMMONWIDGET_H
