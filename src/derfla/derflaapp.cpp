#include "stdafx.h"
#include <QSystemTrayIcon>
#include "extensionmanager.h"
#include "derflawidget.h"
#include "alfredwidget.h"
#include "derflaapp.h"

DerflaApp::DerflaApp(QObject *parent) 
    : QObject(parent)
    , extensionManager_(new ExtensionManager(this))
    , trayIcon_(new QSystemTrayIcon(this))
{
    
}

void DerflaApp::createDerflaWidget()
{
    derflaWidget_ = new DerflaWidget(extensionManager_);
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
