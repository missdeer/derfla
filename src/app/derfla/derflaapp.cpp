#include "stdafx.h"

#include "derflaapp.h"
#include "alfredwidget.h"
#include "commonwidget.h"
#include "derflawidget.h"
#include "extensionmanager.h"

DerflaApp::DerflaApp(QObject *parent) : QObject(parent), extensionManager_(new ExtensionManager), trayIcon_(new QSystemTrayIcon)
{
    extensionManager_->loadAllFromLocal();
    connect(extensionManager_, &ExtensionManager::actionUpdated, this, &DerflaApp::onActionUpdated);
    connect(extensionManager_, &ExtensionManager::emptyAction, this, &DerflaApp::onEmptyAction);
    connect(trayIcon_, &QSystemTrayIcon::activated, this, &DerflaApp::onTrayIconActivated);

    createCommonActions();
    createDonateDerflaActions();
}

DerflaApp::~DerflaApp()
{
    delete trayIcon_;
    delete extensionManager_;
    delete derflaWidget_;
    delete alfredWidget_;
}

void DerflaApp::createWidget()
{
    if (settings_.value("alfredStyleUI").toBool())
    {
        widget_ = createAlfredWidget();
    }
    else
    {
        widget_ = createDerflaWidget();
    }
}

void DerflaApp::show()
{
    widget_->show();
    centerToScreen(widget_);
}

DerflaWidget *DerflaApp::createDerflaWidget()
{
    if (!derflaWidget_)
    {
        derflaWidget_ = new DerflaWidget;
    }
    return derflaWidget_;
}

void DerflaApp::showDerflaWidget()
{
    derflaWidget_->show();
    centerToScreen(derflaWidget_);
}

AlfredWidget *DerflaApp::createAlfredWidget()
{
    if (!alfredWidget_)
    {
        alfredWidget_ = new AlfredWidget();
    }
    return alfredWidget_;
}

void DerflaApp::showAlfredWidget()
{
    alfredWidget_->show();
    centerToScreen(alfredWidget_);
}

void DerflaApp::queryByExtension(const QString &text)
{
    extensionManager_->query(text);
}

void DerflaApp::setCheckedStayOnTopAction(bool checked)
{
    Q_ASSERT(stayOnTopAction_);
    stayOnTopAction_->setChecked(checked);
}

QSettings &DerflaApp::settings()
{
    return settings_;
}

DerflaActionPtr DerflaApp::derflaAction(int index)
{
    if (index >= 0 && index < dal_.size())
    {
        return dal_.at(index);
    }

    if (index < dal_.size() + dalDonate_.size())
    {
        return dalDonate_.at(index - dal_.size());
    }
    return {};
}

void DerflaApp::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
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
    QMessageBox::about(
        widget_,
        tr("Derfla"),
        tr("Derfla is a cross platform productivity application, which boosts your efficiency with hotkeys, keywords, text expansion and more. "
           "Search your Windows and the web, and be more productive with custom actions to control your system.\r\n\r\n"
           "Contact me at https://minidump.info/derfla/ if you have any problem about this tool. Built at " __DATE__ " " __TIME__));
}

void DerflaApp::onPreference()
{
    widget_->onPreference();
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
    std::copy(dal.begin(), dal.end(), std::back_inserter(dal_));
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

#if defined(Q_OS_MAC)
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("Tools");
    QString target = dir.absolutePath() + "/donate";
#elif defined(Q_OS_WIN)
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
    dalDonate_.push_back(daPaypal);
    dalDonate_.push_back(daAlipay);
    dalDonate_.push_back(daWeChatPay);
}

void DerflaApp::centerToScreen(QWidget *widget)
{
    if (!widget)
    {
        return;
    }
    QScreen *screen = QGuiApplication::screenAt(QCursor::pos());
    QSize    size   = screen->availableSize();
    widget->move((size.width() - widget->width()) / 2, (size.height() - widget->height()) / 4);
}

void DerflaApp::createCommonActions()
{
    auto *selectFileAction = new QAction(tr("Select File"), this);
    selectFileAction->setShortcut(tr("Ctrl+O"));
    connect(selectFileAction, &QAction::triggered, this, &DerflaApp::onSelectFile);

    auto *selectFolderAction = new QAction(tr("Select Folder"), this);
    selectFolderAction->setShortcut(tr("Ctrl+D"));
    connect(selectFolderAction, &QAction::triggered, this, &DerflaApp::onSelectFolder);

    auto *loadSkinAction = new QAction(tr("Load &Skin"), this);
    connect(loadSkinAction, &QAction::triggered, this, &DerflaApp::onLoadSkin);

    stayOnTopAction_ = new QAction(tr("Stay On Top"), this);
    stayOnTopAction_->setCheckable(true);
    connect(stayOnTopAction_, &QAction::triggered, this, &DerflaApp::onStayOnTop);

    auto *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, &QAction::triggered, this, &DerflaApp::onQuit);

    auto *showAction = new QAction(tr("Show"), this);
    connect(showAction, &QAction::triggered, this, &DerflaApp::onShowInFront);

    auto *aboutAction = new QAction(tr("About"), this);
    connect(aboutAction, &QAction::triggered, this, &DerflaApp::onAbout);

    auto *homepageAction = new QAction(tr("Homepage"), this);
    connect(homepageAction, &QAction::triggered, []() { QDesktopServices::openUrl(QUrl("https://minidump.info/derfla/")); });

    auto *preferenceAction = new QAction(tr("Preference..."), this);
    connect(preferenceAction, &QAction::triggered, this, &DerflaApp::onPreference);

    auto *trayiconMenu          = new QMenu(nullptr);
    auto *donateMenu            = new QMenu(tr("Donate"), trayiconMenu);
    auto *donateViaPaypalAction = new QAction(QIcon(":rc/paypal.png"), tr("Via Paypal..."), this);
    connect(donateViaPaypalAction, &QAction::triggered, this, &DerflaApp::donateViaPaypal);
    donateMenu->addAction(donateViaPaypalAction);

    auto *donateViaAlipayAction = new QAction(QIcon(":rc/alipay.png"), tr("Via Alipay..."), this);
    connect(donateViaAlipayAction, &QAction::triggered, this, &DerflaApp::donateViaAlipay);
    donateMenu->addAction(donateViaAlipayAction);

    auto *donateViaWeChatAction = new QAction(QIcon(":rc/wechat.png"), tr("Via WeChat Pay..."), this);
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
    trayiconMenu->addAction(stayOnTopAction_);
    trayiconMenu->addAction(preferenceAction);
    trayiconMenu->addMenu(donateMenu);
    trayiconMenu->addSeparator();
    trayiconMenu->addAction(quitAction);

    connect(trayIcon_, &QSystemTrayIcon::activated, this, &DerflaApp::onTrayIconActivated);
    trayIcon_->setContextMenu(trayiconMenu);
    trayIcon_->setIcon(QIcon(":/derfla.ico"));
    trayIcon_->setToolTip(tr("Derfla - Run appliation/script as you want!"));
    trayIcon_->show();
}
qsizetype DerflaApp::derflaActionCount() const
{
    return dal_.size();
}
bool DerflaApp::isEmptyDerflaAction() const
{
    return dal_.empty();
}
void DerflaApp::clearDerflaAction()
{
    dal_.clear();
}
DerflaActionList &DerflaApp::derflaActions()
{
    return dal_;
}
DerflaActionList &DerflaApp::donateDerflaActions()
{
    return dalDonate_;
}
ExtensionManager *DerflaApp::extensionManager()
{
    return extensionManager_;
}
void DerflaApp::executeAction(const DerflaActionPtr &action)
{
    actionExecutor_(action);
}
