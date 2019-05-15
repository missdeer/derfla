#include "stdafx.h"
#include <QSystemTrayIcon>
#include "extensionmanager.h"
#include "derflawidget.h"
#include "alfredwidget.h"
#include "autoupdater.h"
#include "derflaapp.h"

DerflaApp::DerflaApp(QObject *parent) 
    : QObject(parent)
    , extensionManager_(new ExtensionManager)
    , trayIcon_(new QSystemTrayIcon)
{
    extensionManager_->loadAllFromLocal();
    extensionManager_->loadAllFromCache();
}

DerflaApp::~DerflaApp()
{
    if (trayIcon_)
        delete trayIcon_;
    if (extensionManager_)
        delete extensionManager_;
    if (derflaWidget_)
        delete derflaWidget_;
    if (alfredWidget_)
        delete alfredWidget_;
    if (autoUpdater_)
        delete autoUpdater_;
}

void DerflaApp::createWidget()
{
    if (settings_.value("alfredStyleUI").toBool())
    {
        createAlfredWidget();
    }
    else 
    {
        createDerflaWidget();
    }
}

void DerflaApp::show()
{
    if (settings_.value("alfredStyleUI").toBool())
    {
        showAlfredWidget();
    }
    else 
    {
        showDerflaWidget();
    }
}

void DerflaApp::createDerflaWidget()
{
    if (!derflaWidget_)
        derflaWidget_ = new DerflaWidget;
}

void DerflaApp::showDerflaWidget()
{
    derflaWidget_->show();
    CenterToScreen(derflaWidget_);
}

void DerflaApp::createAlfredWidget()
{
    if (!alfredWidget_)
        alfredWidget_ = new AlfredWidget();
}

void DerflaApp::showAlfredWidget()
{
    alfredWidget_->show();
    CenterToScreen(alfredWidget_);
}

void DerflaApp::autoUpdate()
{
    if (settings_.value("autoupdate", true).toBool())
    {
        if (!autoUpdater_)
            autoUpdater_ = AutoUpdater::createAutoUpdate();
        autoUpdater_->checkForUpdates();
    }
}

void DerflaApp::checkForUpdates()
{
    autoUpdate();
}

QSettings &DerflaApp::settings()
{
    return settings_;
}

void DerflaApp::CenterToScreen(QWidget *widget)
{
    if (!widget)
        return;
    QScreen* scr = qApp->primaryScreen();
    QSize sz = scr->availableSize();
    widget->move((sz.width() - widget->width())/2, (sz.height()-widget->height())/2);
}
