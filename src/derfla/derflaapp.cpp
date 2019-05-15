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
    CenterToScreen(derflaWidget_);
}

void DerflaApp::createAlfredWidget()
{
    alfredWidget_ = new AlfredWidget();
}

void DerflaApp::showAlfredWidget()
{
    alfredWidget_->show();
    CenterToScreen(alfredWidget_);
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

void DerflaApp::CenterToScreen(QWidget *widget)
{
    if (!widget)
        return;
    QScreen* scr = qApp->primaryScreen();
    QSize sz = scr->availableSize();
    int desk_x = sz.width();
    int desk_y = sz.height();
    int x = widget->width();
    widget->move(desk_x / 2 - x / 2, 
                 (desk_y - beginheight - rowsize * (MAXPRINTSIZE - 1)) * widget->height());
}
