#include "stdafx.h"
#include "commonwidget.h"
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
    connect(extensionManager_, &ExtensionManager::actionUpdated, this, &DerflaApp::onActionUpdated);
    connect(extensionManager_, &ExtensionManager::emptyAction, this, &DerflaApp::onEmptyAction);
    connect(trayIcon_, &QSystemTrayIcon::activated, this, &DerflaApp::onTrayIconActivated);
    
    createCommonActions();
    createDonateDerflaActions();
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
        widget_ = alfredWidget_;
    }
    else 
    {
        createDerflaWidget();
        widget_ = derflaWidget_;
    }
}

void DerflaApp::show()
{
    widget_->show();
    centerToScreen(widget_);
}

void DerflaApp::createDerflaWidget()
{
    if (!derflaWidget_)
        derflaWidget_ = new DerflaWidget;
}

void DerflaApp::showDerflaWidget()
{
    derflaWidget_->show();
    centerToScreen(derflaWidget_);
}

void DerflaApp::createAlfredWidget()
{
    if (!alfredWidget_)
        alfredWidget_ = new AlfredWidget();
}

void DerflaApp::showAlfredWidget()
{
    alfredWidget_->show();
    centerToScreen(alfredWidget_);
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

void DerflaApp::queryByExtension(const QString &text) 
{ 
    extensionManager_->query(text);
}

QSettings &DerflaApp::settings()
{
    return settings_;
}

DerflaActionPtr DerflaApp::derflaAction(int index) 
{ 
    if (index >= 0 && index < dal_.length())
        return dal_.at(index);
    else if (index < dal_.length() + dalDonate_.length())
        return dalDonate_.at(index - dal_.length());
    return DerflaActionPtr();
}

void DerflaApp::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::DoubleClick:
        widget_->onShowInFront();
        break;
    default:
        break;
    }
}

void DerflaApp::onLoadSkin()
{
    widget_->onLoadSkin();
}

void DerflaApp::onInstallExtension()
{
    QString fileName = QFileDialog::getOpenFileName(widget_,
		tr("Install Derfla Extension"),
		"",
        tr("Derfla Extension Package (*.derflaextension);;Derfla Extension Configuration (extension.cfg);;All files (*.*)"));
    if (!QFile::exists(fileName))
        return;
    try {
        extensionManager_->installExtension(fileName);
    }
    catch(std::runtime_error& e) {
        QMessageBox::warning(widget_, tr("Installing extension failed"), QString::fromLatin1(e.what()), QMessageBox::Ok);
    }
}

void DerflaApp::onStayOnTop()
{
    widget_->onStayOnTop();
}

void DerflaApp::onShowInFront()
{
    widget_->onShowInFront();
}

void DerflaApp::onQuit()
{
    QCoreApplication::quit();
}

void DerflaApp::onSelectFile()
{
    widget_->onSelectFile();
}

void DerflaApp::onSelectFolder()
{
    widget_->onSelectFolder();
}

void DerflaApp::onAbout()
{
    QMessageBox::about(widget_, tr("Derfla"), tr(
                           "Derfla is a cross platform productivity application, which boosts your efficiency with hotkeys, keywords, text expansion and more. "
                           "Search your Windows and the web, and be more productive with custom actions to control your system.\r\n\r\n"
                           "Contact me at https://minidump.info/derfla/ if you have any problem about this tool. Built at " __DATE__ " " __TIME__
                           ));
}

void DerflaApp::onPreference()
{
    widget_->onPreference();
}

void DerflaApp::onCheckUpdates()
{
    checkForUpdates();
}

void DerflaApp::donateViaPaypal()
{
    actionExecutor_(dalDonate_[0]);
}

void DerflaApp::donateViaAlipay()
{
    actionExecutor_(dalDonate_[1]);
}

void DerflaApp::donateViaWeChatPay()
{
    actionExecutor_(dalDonate_[2]);
}

void DerflaApp::onActionUpdated(DerflaActionList &dal)
{
    dal_.append(dal);
    emit actionUpdated();
}

void DerflaApp::onEmptyAction()
{
    emit emptyAction();
}

void DerflaApp::createDonateDerflaActions()
{
    DerflaActionPtr daPaypal(new DerflaAction);
    daPaypal->setTitle(tr("Donate to support me"));
    daPaypal->setDescription(tr("Donate via Paypal"));
    daPaypal->setIcon(QIcon(":rc/paypal.png"));
    daPaypal->setActionType("openUrl");
    daPaypal->setTarget("https://www.paypal.me/dfordsoft");

#if defined (Q_OS_MAC)
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("Tools");
    QString target = dir.absolutePath() + "/donate";
#elif defined (Q_OS_WIN)
    QString target = QCoreApplication::applicationDirPath() + "/donate.exe";
#else
    QString target = QCoreApplication::applicationDirPath() + "/donate";
#endif
    DerflaActionPtr daAlipay(new DerflaAction);
    daAlipay->setTitle(tr("Donate to support me"));
    daAlipay->setDescription(tr("Donate via Alipay"));
    daAlipay->setIcon(QIcon(":rc/alipay.png"));
    daAlipay->setActionType("shellExecute");
    daAlipay->setTarget(target);
    daAlipay->setArguments("--alipay");
    daAlipay->setWorkingDirectory(QCoreApplication::applicationDirPath());
    DerflaActionPtr daWeChatPay(new DerflaAction);
    daWeChatPay->setTitle(tr("Donate to support me"));
    daWeChatPay->setDescription(tr("Donate via WeChat pay"));
    daWeChatPay->setIcon(QIcon(":rc/wechat.png"));
    daWeChatPay->setActionType("shellExecute");
    daWeChatPay->setTarget(target);
    daWeChatPay->setArguments("--wechat");
    daWeChatPay->setWorkingDirectory(QCoreApplication::applicationDirPath());
    dalDonate_ << daPaypal << daAlipay << daWeChatPay;
}

void DerflaApp::centerToScreen(QWidget *widget)
{
    if (!widget)
        return;
    QScreen* scr = qApp->primaryScreen();
    QSize sz = scr->availableSize();
    widget->move((sz.width() - widget->width())/2, (sz.height()-widget->height())/4);
}

void DerflaApp::createCommonActions()
{    
    QAction *selectFileAction = new QAction(tr("Select File"), this);
    selectFileAction->setShortcut(tr("Ctrl+O"));
    connect(selectFileAction, &QAction::triggered, this, &DerflaApp::onSelectFile);

    QAction *selectFolderAction = new QAction(tr("Select Folder"), this);
    selectFolderAction->setShortcut(tr("Ctrl+D"));
    connect(selectFolderAction, &QAction::triggered, this, &DerflaApp::onSelectFolder);

    QAction *loadSkinAction = new QAction(tr("Load &Skin"), this);
    connect(loadSkinAction, &QAction::triggered, this, &DerflaApp::onLoadSkin);

    QAction *installExtensionAction = new QAction(tr("&Install Extension"), this);
    connect(installExtensionAction, &QAction::triggered, this, &DerflaApp::onInstallExtension);

    stayOnTopAction_ = new QAction(tr("Stay On Top"), this);
    stayOnTopAction_->setCheckable(true);
    connect(stayOnTopAction_, &QAction::triggered, this, &DerflaApp::onStayOnTop);

    QAction *checkUpdatesAction = new  QAction(tr("Check Updates..."), this);
    connect(checkUpdatesAction, &QAction::triggered, this, &DerflaApp::onCheckUpdates);

    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, &QAction::triggered, this, &DerflaApp::onQuit);

    QAction *showAction = new QAction(tr("Show"), this);
    connect(showAction, &QAction::triggered, this, &DerflaApp::onShowInFront);

    QAction *aboutAction = new QAction(tr("About"), this);
    connect(aboutAction, &QAction::triggered, this, &DerflaApp::onAbout);

    QAction *homepageAction = new QAction(tr("Homepage"), this);
    connect(homepageAction, &QAction::triggered, [](){
        QDesktopServices::openUrl(QUrl("https://minidump.info/derfla/"));
    });

    QAction *preferenceAction = new QAction(tr("Preference..."), this);
    connect(preferenceAction, &QAction::triggered, this, &DerflaApp::onPreference);
    
    QMenu* trayiconMenu = new QMenu(nullptr);
    QMenu* donateMenu = new QMenu(tr("Donate"), trayiconMenu);
    QAction *donateViaPaypalAction = new QAction(QIcon(":rc/paypal.png"), tr("Via Paypal..."), this);
    connect(donateViaPaypalAction, &QAction::triggered, this, &DerflaApp::donateViaPaypal);
    donateMenu->addAction(donateViaPaypalAction);

    QAction *donateViaAlipayAction = new QAction(QIcon(":rc/alipay.png"), tr("Via Alipay..."), this);
    connect(donateViaAlipayAction, &QAction::triggered, this, &DerflaApp::donateViaAlipay);
    donateMenu->addAction(donateViaAlipayAction);

    QAction *donateViaWeChatAction = new QAction(QIcon(":rc/wechat.png"), tr("Via WeChat Pay..."), this);
    connect(donateViaWeChatAction, &QAction::triggered, this, &DerflaApp::donateViaWeChatPay);
    donateMenu->addAction(donateViaWeChatAction);

    trayiconMenu->addAction(showAction);
    trayiconMenu->addAction(aboutAction);
    trayiconMenu->addAction(homepageAction);
    trayiconMenu->addSeparator();
    trayiconMenu->addAction(selectFileAction);
    trayiconMenu->addAction(selectFolderAction);
    trayiconMenu->addSeparator();
    trayiconMenu->addAction(loadSkinAction);
    trayiconMenu->addAction(installExtensionAction);
    trayiconMenu->addAction(stayOnTopAction_);
    trayiconMenu->addAction(checkUpdatesAction);
    trayiconMenu->addAction(preferenceAction);
    trayiconMenu->addMenu(donateMenu);
    trayiconMenu->addSeparator();
    trayiconMenu->addAction(quitAction);
    
    connect(trayIcon_, &QSystemTrayIcon::activated, this, &DerflaApp::onTrayIconActivated);
    trayIcon_->setContextMenu(trayiconMenu);
    trayIcon_->setIcon(QIcon(":/derfla.ico"));
    trayIcon_->setToolTip(tr("Derfla - Accelerate your keyboard!"));
    trayIcon_->show();
}
