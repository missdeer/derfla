#include "stdafx.h"
#include "derflaapp.h"
#include "preferencedialog.h"
#include "commonwidget.h"

CommonWidget::CommonWidget(QWidget *parent) 
    : QWidget(parent)
    #if defined(Q_OS_WIN)
    , hotkeyManager_(new QGlobalShortcut(this))
    #else
    , hotkeyManager_(new UGlobalHotkeys(this))
    #endif
{
    QSettings &settings = derflaApp->settings();
    stayOnTop_ = settings.value("stayOnTop", false).toBool();
    
    QString keySequence = settings.value("hotkey", "Alt+Space").toString();
#if defined(Q_OS_WIN)
    hotkeyManager_->setKey(QKeySequence(keySequence));
    connect(hotkeyManager_, &QGlobalShortcut::activated, derflaApp,  &DerflaApp::onShowInFront);
#else
    hotkeyManager_->registerHotkey(keySequence);
    connect(hotkeyManager_, &UGlobalHotkeys::activated, derflaApp,  &DerflaApp::onShowInFront);
#endif
}

void CommonWidget::onLoadSkin()
{
    
}

void CommonWidget::onStayOnTop()
{
    
}

void CommonWidget::onShowInFront()
{
    if (isHidden())
        show();
    activateWindow();
    raise();
}

void CommonWidget::onSelectFile()
{

}

void CommonWidget::onSelectFolder()
{
    
}

bool CommonWidget::onPreference()
{
#if defined(Q_OS_WIN)
    hotkeyManager_->unsetKey();
#else
    hotkeyManager_->unregisterHotkey();
#endif
    PreferenceDialog dlg(this);
    return (dlg.exec() == QDialog::Accepted);
}

void CommonWidget::hideCandidateList()
{

}
