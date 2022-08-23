#include "stdafx.h"

#include "commonwidget.h"
#include "derflaapp.h"
#include "preferencedialog.h"
#if defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#    include <QtPlatformHeaders/QWindowsWindowFunctions>
#endif

CommonWidget::CommonWidget(QWidget *parent)
    : QWidget(parent)
#if defined(Q_OS_WIN)
      ,
      hotkeyManager_(new QGlobalShortcut(this))
#else
      ,
      hotkeyManager_(new UGlobalHotkeys(this))
#endif
{
    QSettings &settings = derflaApp->settings();
    stayOnTop_          = settings.value("stayOnTop", false).toBool();

    QString keySequence = settings.value("hotkey", "Alt+Space").toString();
#if defined(Q_OS_WIN)
#    if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QWindowsWindowFunctions::setWindowActivationBehavior(QWindowsWindowFunctions::AlwaysActivateWindow);
#    endif
    hotkeyManager_->setKey(QKeySequence(keySequence));
    connect(hotkeyManager_, &QGlobalShortcut::activated, derflaApp, &DerflaApp::onShowInFront);
#else
    hotkeyManager_->registerHotkey(keySequence);
    connect(hotkeyManager_, &UGlobalHotkeys::activated, derflaApp, &DerflaApp::onShowInFront);
#endif
}

void CommonWidget::onLoadSkin() {}

void CommonWidget::onStayOnTop()
{
    if (stayOnTop_)
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    else
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    onShowInFront();

    QSettings &settings = derflaApp->settings();
    settings.setValue("stayOnTop", stayOnTop_);
    settings.sync();
}

void CommonWidget::onShowInFront()
{
    if (isHidden())
        show();
    activateWindow();
    raise();
}

void CommonWidget::onSelectFile() {}

void CommonWidget::onSelectFolder() {}

bool CommonWidget::onPreference()
{
#if defined(Q_OS_WIN)
    hotkeyManager_->unsetKey();
#else
    hotkeyManager_->unregisterHotkey();
#endif
    PreferenceDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return false;

    QSettings &settings = derflaApp->settings();
    stayOnTop_          = settings.value("stayOnTop", false).toBool();
    derflaApp->setCheckedStayOnTopAction(stayOnTop_);
    if (stayOnTop_)
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    else
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);

    onShowInFront();

    if (settings.value("autostart", false).toBool())
    {
#if defined(Q_OS_WIN)
        QString   key = "Derfla";
        QSettings registrySettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        registrySettings.remove(key);
        registrySettings.setValue(key, QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        registrySettings.sync();
#elif defined(Q_OS_MAC)
        // Remove any existing login entry for this app first, in case there was one
        // from a previous installation, that may be under a different launch path.

        QStringList args;
        args << "-e tell application \"System Events\" to delete login item\"Derfla\"";

        QProcess::execute("osascript", args);

        QDir dir(QCoreApplication::applicationDirPath());
        dir.cdUp();
        dir.cdUp();
        QString absolutePath = dir.absolutePath();
        // absolutePath will contain a "/" at the end,
        // but we want the clean path to the .app bundle
        if (absolutePath.length() > 0 && absolutePath.right(1) == "/")
        {
            absolutePath.chop(1);
        }

        // Now install the login item, if needed.
        args.clear();
        args << "-e tell application \"System Events\" to make login item at end "
                "with properties {path:\"" +
                    absolutePath + "\", hidden:false}";

        QProcess::execute("osascript", args);
#endif
    }
    else
    {
#if defined(Q_OS_WIN)
        QString   key = "Derfla";
        QSettings registrySettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        registrySettings.remove(key);
        registrySettings.sync();
#elif defined(Q_OS_MAC)
        // Remove any existing login entry for this app first, in case there was one
        // from a previous installation, that may be under a different launch path.

        QStringList args;
        args << "-e tell application \"System Events\" to delete login item\"Derfla\"";

        QProcess::execute("osascript", args);
#endif
    }

    QString keySequence = settings.value("hotkey", "Alt+Space").toString();
#if defined(Q_OS_WIN)
    hotkeyManager_->setKey(QKeySequence(keySequence));
#else
    hotkeyManager_->registerHotkey(keySequence);
#endif
    return true;
}

void CommonWidget::hideCandidateList() {}
