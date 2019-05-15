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

void DerflaApp::createDerflaWidget()
{
    derflaWidget_ = new DerflaWidget;
}

void DerflaApp::showDerflaWidget()
{
    derflaWidget_->show();
    
    QScreen* scr = qApp->primaryScreen();
    QSize sz = scr->availableSize();
    derflaWidget_->move((sz.width() - derflaWidget_->width())/2, (sz.height()-derflaWidget_->height())/2);
}

void DerflaApp::createAlfredWidget()
{
    alfredWidget_ = new AlfredWidget();
}

void DerflaApp::showAlfredWidget()
{
    alfredWidget_->show();
}

void DerflaApp::autoUpdate()
{
    autoUpdater_ = AutoUpdater::createAutoUpdate();
    QSettings settings;
    if (settings.value("autoupdate", true).toBool())
        autoUpdater_->checkForUpdates();
}

void DerflaApp::checkForUpdates()
{
    if (autoUpdater_)
        autoUpdater_->checkForUpdates();
}