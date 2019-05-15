#include "stdafx.h"
#include <private/qzipreader_p.h>
#include "derflaapp.h"
#include "skinmanager.h"
#if defined(Q_OS_WIN)
#include "qglobalshortcut.h"
#else
#include "uglobalhotkeys.h"
#endif
#include "charlineedit.h"
#include "extensionmanager.h"
#include "candidatelist.h"
#include "preferencedialog.h"
#include "derflawidget.h"

DerflaWidget::DerflaWidget(QWidget *parent)
    : QWidget(parent)
    , mouseMovePos_(0, 0)
    , candidateDelayTimer_(new QTimer(this))
    , input_(new CharLineEdit(this))
    , candidateList_(new CandidateList(derflaApp->extensionManager_, this))
    #if defined(Q_OS_WIN)
    , hotkeyManager_(new QGlobalShortcut(this))
    #else
    , hotkeyManager_(new UGlobalHotkeys(this))
    #endif
    , skinManager_(new SkinManager)
{
#if defined(Q_OS_WIN)
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool );
#else
    setWindowFlags(Qt::FramelessWindowHint );
#endif
    setAttribute(Qt::WA_InputMethodEnabled);
    setAttribute(Qt::WA_TranslucentBackground);

    setFocusPolicy(Qt::ClickFocus);

    setWindowIcon(QIcon(":/derfla.png"));

    candidateDelayInterval_ = derflaApp->settings_.value("interval", 0).toInt();
    stayOnTop_ = derflaApp->settings_.value("stayOnTop", false).toBool();

    QString skinPath = derflaApp->settings_.value("skin", ":/skins/derfla.derflaskin").toString();
    if (!applySkin(skinPath))
    {
        qWarning() << "loading skin failed:" << skinPath;
        if (!applySkin(":/skins/derfla.derflaskin"))
        {
            qCritical() << "loading skin failed";
            return;
        }
    }
#ifdef Q_WS_MAC
    QMacStyle::setFocusRectPolicy(input, QMacStyle::FocusDisabled);
#endif

    connect(candidateList_, &CandidateList::done, this, &DerflaWidget::onCandidateListDone);
    connect(candidateList_, &CandidateList::keyPressedEvent, this, &DerflaWidget::onKeyPressed);
//    QAction *logoAction = new QAction(tr("Input"), this);
//    logoAction->setIcon(QIcon(":/derfla.ico"));
//    input_->addAction(logoAction, QLineEdit::ActionPosition::TrailingPosition);
    input_->setObjectName("input");
    input_->setClearButtonEnabled(false);
    connect(input_, &CharLineEdit::keyPressed, this, &DerflaWidget::keyPressEvent);
    connect(input_, &QLineEdit::textChanged, this, &DerflaWidget::onInputChanged);
    connect(input_, &CharLineEdit::focusOut, this, &DerflaWidget::onFocusOut);
    connect(input_, &CharLineEdit::focusIn, this, &DerflaWidget::onFocusIn);

    QAction *selectFileAction = new QAction(tr("Select File"), this);
    selectFileAction->setShortcut(tr("Ctrl+O"));
    connect(selectFileAction, &QAction::triggered, this, &DerflaWidget::onSelectFile);

    QAction *selectFolderAction = new QAction(tr("Select Folder"), this);
    selectFolderAction->setShortcut(tr("Ctrl+D"));
    connect(selectFolderAction, &QAction::triggered, this, &DerflaWidget::onSelectFolder);

    QAction *loadSkinAction = new QAction(tr("Load &Skin"), this);
    connect(loadSkinAction, &QAction::triggered, this, &DerflaWidget::onLoadSkin);

    QAction *installExtensionAction = new QAction(tr("&Install Extension"), this);
    connect(installExtensionAction, &QAction::triggered, this, &DerflaWidget::onInstallExtension);

    stayOnTopAction_ = new QAction(tr("Stay On Top"), this);
    stayOnTopAction_->setCheckable(true);
    if (stayOnTop_)
    {
        stayOnTopAction_->setChecked(true);
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    }
    connect(stayOnTopAction_, &QAction::triggered, this, &DerflaWidget::onStayOnTop);

    QAction *checkUpdatesAction = new  QAction(tr("Check Updates..."), this);
    connect(checkUpdatesAction, &QAction::triggered, this, &DerflaWidget::onCheckUpdates);

    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, &QAction::triggered, this, &DerflaWidget::onQuit);

    setContextMenuPolicy(Qt::CustomContextMenu);

    QAction *showAction = new QAction(tr("Show"), this);
    connect(showAction, &QAction::triggered, this, &DerflaWidget::onShowInFront);

    QAction *aboutAction = new QAction(tr("About"), this);
    connect(aboutAction, &QAction::triggered, this, &DerflaWidget::onAbout);

    QAction *homepageAction = new QAction(tr("Homepage"), this);
    connect(homepageAction, &QAction::triggered, [](){
        QDesktopServices::openUrl(QUrl("https://minidump.info/derfla/"));
    });

    QAction *preferenceAction = new QAction(tr("Preference..."), this);
    connect(preferenceAction, &QAction::triggered, this, &DerflaWidget::onPreference);

    QMenu* donateMenu = new QMenu(tr("Donate"), this);
    QAction *donateViaPaypalAction = new QAction(QIcon(":rc/paypal.png"), tr("Via Paypal..."), this);
    connect(donateViaPaypalAction, &QAction::triggered, candidateList_, &CandidateList::donateViaPaypal);
    donateMenu->addAction(donateViaPaypalAction);

    QAction *donateViaAlipayAction = new QAction(QIcon(":rc/alipay.png"), tr("Via Alipay..."), this);
    connect(donateViaAlipayAction, &QAction::triggered, candidateList_, &CandidateList::donateViaAlipay);
    donateMenu->addAction(donateViaAlipayAction);

    QAction *donateViaWeChatAction = new QAction(QIcon(":rc/wechat.png"), tr("Via WeChat Pay..."), this);
    connect(donateViaWeChatAction, &QAction::triggered, candidateList_, &CandidateList::donateViaWeChatPay);
    donateMenu->addAction(donateViaWeChatAction);

    QMenu* trayiconMenu = new QMenu(this);
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
    
    connect(derflaApp->trayIcon_, &QSystemTrayIcon::activated, this, &DerflaWidget::onTrayIconActivated);
    derflaApp->trayIcon_->setContextMenu(trayiconMenu);
    derflaApp->trayIcon_->setIcon(QIcon(":/derfla.ico"));
    derflaApp->trayIcon_->setToolTip(tr("Derfla - Accelerate your keyboard!"));
    derflaApp->trayIcon_->show();

    connect(this, &QWidget::customContextMenuRequested, this, &DerflaWidget::onCustomContextMenuRequested);
    connect(candidateDelayTimer_, &QTimer::timeout, this, &DerflaWidget::onCandidateDelayTimer);
    candidateDelayTimer_->setSingleShot(true);
    
    QString keySequence = derflaApp->settings_.value("hotkey", "Alt+Space").toString();
#if defined(Q_OS_WIN)
    hotkeyManager_->setKey(QKeySequence(keySequence));
    connect(hotkeyManager_, &QGlobalShortcut::activated, this,  &DerflaWidget::onShowInFront);
#else
    hotkeyManager_->registerHotkey(keySequence);
    connect(hotkeyManager_, &UGlobalHotkeys::activated, this,  &DerflaWidget::onShowInFront);
#endif
}

DerflaWidget::~DerflaWidget()
{
    candidateDelayTimer_->stop();
    delete skinManager_;
}

void DerflaWidget::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton))
    {
        move(mapToParent(event->pos() - mouseMovePos_));
    }
}

void DerflaWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mouseMovePos_ = event->pos();
    }
}

void DerflaWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        mouseMovePos_ = QPoint(0, 0);
    }
}

void DerflaWidget::paintEvent(QPaintEvent* event)
{
    QStyleOption styleOption;
    styleOption.init(this);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    style()->drawPrimitive(QStyle::PE_Widget, &styleOption, &painter, this);
    QSize size(skinManager_->backgroundImage().size());

    if (size.width() > widgetMinWidth_)
        painter.drawPixmap(0, 0, skinManager_->backgroundImage());
    else
    {
        painter.drawPixmap(0, 0, skinManager_->leftPartBackgroundImage());
        painter.drawPixmap(skinManager_->leftPartBackgroundImage().width(), 0, skinManager_->midPartBackgroundImage());
        painter.drawPixmap(skinManager_->leftPartBackgroundImage().width() + skinManager_->midPartBackgroundImage().width(), 0, skinManager_->rightPartBackgroundImage());
    }
    QWidget::paintEvent(event);
}

void DerflaWidget::moveEvent(QMoveEvent* /*event*/)
{
    if (candidateList_ && candidateList_->isVisible())
        candidateList_->move(mapToGlobal(QPoint(input_->x(), input_->y() + input_->height())));
}

void DerflaWidget::keyPressEvent(QKeyEvent *event)
{
    //qDebug() << "DerflaWidget::keyPressEvent 0:" << event->key();
    if (event->key() == Qt::Key_Escape)
    {
        //qDebug() << "DerflaWidget::keyPressEvent escape" << candidateList_->isVisible();
        if (candidateList_->isVisible())
            candidateList_->hide();
        else
        {
            if (input_->text().isEmpty() && !stayOnTop_)
                hide();
            else
                input_->setText("");
        }
    }
    else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        doEnter();
    }
    else if (event->key() == Qt::Key_Down || event->key() == Qt::Key_PageDown ||
             event->key() == Qt::Key_Up || event->key() == Qt::Key_PageUp)
    {
        //qDebug() << "DerflaWidget::keyPressEvent 1:" << event->key();
        if (candidateList_->isVisible())
        {
            //qDebug() << "DerflaWidget::keyPressEvent 2:" << event->key();
            candidateList_->activateWindow();
            candidateList_->setActiveWindowFlag(true);
            qApp->sendEvent(candidateList_, event);
        }
    }
    else if ((event->key() == Qt::Key_Tab || event->key() == Qt::Key_Backspace) && event->modifiers() == Qt::ShiftModifier)
    {
        doBackTab();
        processKey();
    }
    else if (event->key() == Qt::Key_Tab)
    {
        doTab();
        processKey();
    }
    else if (event->key() == Qt::Key_Slash || event->key() == Qt::Key_Backslash)
    {
        //        if (inputData.count() > 0 && inputData.last().hasLabel(LABEL_FILE) &&
        //                searchResults.count() > 0 && searchResults[0].id == HASH_LAUNCHYFILE)
        //            doTab();
        processKey();
    }
    else if (event->key()== Qt::Key_Insert && event->modifiers() == Qt::ShiftModifier)
    {
        // ensure pasting text with Shift+Insert also parses input
        // longer term parsing should be done using the TextChanged event
        processKey();
    }
    else if (event->text().length() > 0)
    {
        if (!input_->hasFocus())
        {
            input_->setFocus();
            qApp->sendEvent(input_, event);
        }
        // process any other key with character output
        event->ignore();
        processKey();
    }
}

void DerflaWidget::focusOutEvent(QFocusEvent *)
{
    if (!input_->hasFocus() && candidateList_->isVisible() && !candidateList_->isActiveWindow())
        hideCandidateList();
}

void DerflaWidget::focusInEvent(QFocusEvent *e)
{
    onFocusIn(e);
}

void DerflaWidget::onInputChanged(const QString &text)
{
    //qDebug() <<  "DerflaWidget::inputChanged:" << input_->text();
    if (text.isEmpty())
    {
        hideCandidateList();
        stopWaiting();
        candidateDelayTimer_->stop();
    }
    else
    {
        candidateDelayTimer_->start(candidateDelayInterval_);
    }
}

void DerflaWidget::onKeyPressed(QKeyEvent *e)
{
    //qDebug() << "DerflaWidget::keyPressed" << e;
//    if ( e->key() != Qt::Key_Escape)
//        hideCandidateList();
    activateWindow();
    raise();
    qApp->sendEvent(this, e);
}

void DerflaWidget::onFocusOut(QFocusEvent *)
{
    // input lost focus
    if (!hasFocus() && candidateList_->isVisible() && !candidateList_->isActiveWindow())
        hideCandidateList();
}

void DerflaWidget::onFocusIn(QFocusEvent *)
{
    // input got focus
    if (!candidateList_->isVisible() && !input_->text().isEmpty())
    {
        candidateDelayTimer_->start(candidateDelayInterval_);
    }
}

void DerflaWidget::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::DoubleClick:
        if (isHidden())
            show();
        activateWindow();
        raise();
        break;
    default:
        break;
    }
}

void DerflaWidget::onLoadSkin()
{    
    hideCandidateList();
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Load Derfla Skin"),
        "",
        tr("Derfla Skin Package (*.derflaskin);;"
           "Derfla Skin Configuration (*.xml);;"
           "Baidu Input Method Skin File (*.bps);;"
           "Sogou Input Method for Windows Skin File (*.ssf);;"
           "Sogou Input Method for macOS Skin File (*.mssf);;"
           "All Supported Files (*.derflaskin *.xml *.bps *.ssf *.mssf);;"
           "All files (*.*)"));
    if (!QFile::exists(fileName))
        return;
    if (applySkin(fileName))
    {
        derflaApp->settings_.setValue("skin", fileName);
        derflaApp->settings_.sync();
    }
}

void DerflaWidget::onInstallExtension()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Install Derfla Extension"),
		"",
        tr("Derfla Extension Package (*.derflaextension);;Derfla Extension Configuration (extension.cfg);;All files (*.*)"));
    if (!QFile::exists(fileName))
        return;
    try {
        derflaApp->extensionManager_->installExtension(fileName);
    }
    catch(std::runtime_error& e) {
        QMessageBox::warning(this, tr("Installing extension failed"), QString::fromLatin1(e.what()), QMessageBox::Ok);
    }
}

void DerflaWidget::onStayOnTop()
{
    QAction *action = qobject_cast<QAction*>(sender());
    stayOnTop_ = !stayOnTop_;
    action->setChecked(stayOnTop_);
    if (stayOnTop_)
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    else
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    onShowInFront();

    derflaApp->settings_.setValue("stayOnTop", stayOnTop_);
    derflaApp->settings_.sync();
}

void DerflaWidget::onShowInFront()
{
    if (isHidden())
        show();
    activateWindow();
    raise();
}

void DerflaWidget::onCandidateListDone()
{
    hideCandidateList();
    input_->setText("");
}

void DerflaWidget::onQuit()
{
    qApp->quit();
}

void DerflaWidget::onCandidateDelayTimer()
{
    showCandidateList();
}

void DerflaWidget::onSelectFile()
{
    if (!isVisible())
        return;
    hideCandidateList();
    QString fileName = QFileDialog::getOpenFileName(this);
    if (fileName.isEmpty())
        return;

    QString text = input_->text();
    if (!text.isEmpty() && !text.endsWith(" "))
        text.append(" ");
    text.append(QDir::toNativeSeparators(fileName));
    input_->setText(text);
}

void DerflaWidget::onSelectFolder()
{
    if (!isVisible())
        return;

    hideCandidateList();
    QString fileName = QFileDialog::getExistingDirectory(this);
    if (fileName.isEmpty())
        return;

    QString text = input_->text();
    if (!text.isEmpty() && !text.endsWith(" "))
        text.append(" ");
    text.append(QDir::toNativeSeparators(fileName));
    input_->setText(text);
}

void DerflaWidget::onAbout()
{
    QMessageBox::about(this, tr("Derfla"), tr(
                           "Derfla is a cross platform productivity application, which boosts your efficiency with hotkeys, keywords, text expansion and more. "
                           "Search your Windows and the web, and be more productive with custom actions to control your system.\r\n\r\n"
                           "Contact me at https://minidump.info/derfla/ if you have any problem about this tool. Built at " __DATE__ " " __TIME__
                           ));
}

void DerflaWidget::onPreference()
{
#if defined(Q_OS_WIN)
    hotkeyManager_->unsetKey();
#else
    hotkeyManager_->unregisterHotkey();
#endif
    PreferenceDialog dlg(derflaApp->extensionManager_->extensions(), this);
    if (dlg.exec() == QDialog::Accepted)
    {
        candidateDelayInterval_ = derflaApp->settings_.value("interval", 0).toInt();

        QString skinPath = derflaApp->settings_.value("skin", ":/skins/derfla.derflaskin").toString();
        if (!applySkin(skinPath))
        {
            qWarning() << "loading skin failed:" << skinPath;
            if (!applySkin(":/skins/derfla.derflaskin"))
            {
                qCritical() << "loading skin failed";
                return;
            }
        }

        stayOnTop_ = derflaApp->settings_.value("stayOnTop", false).toBool();
        stayOnTopAction_->setChecked(stayOnTop_);
        if (stayOnTop_)
            setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        else
            setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);

        onShowInFront();

        if (derflaApp->settings_.value("autostart", false).toBool())
        {
#if defined(Q_OS_WIN)
            QString key = "Derfla";
            QSettings registrySettings(
                "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                QSettings::NativeFormat);
            registrySettings.remove(key);
            registrySettings.setValue(key, QDir::toNativeSeparators(qApp->applicationFilePath()));
            registrySettings.sync();
#elif defined(Q_OS_MAC)
            // Remove any existing login entry for this app first, in case there was one
            // from a previous installation, that may be under a different launch path.

            QStringList args;
            args << "-e tell application \"System Events\" to delete login item\"Derfla\"";

            QProcess::execute("osascript", args);

            QDir dir(qApp->applicationDirPath());
            dir.cdUp();
            dir.cdUp();
            QString absolutePath = dir.absolutePath();
            // absolutePath will contain a "/" at the end,
            // but we want the clean path to the .app bundle
            if ( absolutePath.length() > 0 && absolutePath.right(1) == "/" ) {
                absolutePath.chop(1);
            }

            // Now install the login item, if needed.
            args.clear();
            args << "-e tell application \"System Events\" to make login item at end "
                    "with properties {path:\"" + absolutePath + "\", hidden:false}";

            QProcess::execute("osascript", args);
#endif
        }
        else
        {
#if defined(Q_OS_WIN)
            QString key = "Derfla";
            QSettings registrySettings(
                "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                QSettings::NativeFormat);
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

    }
    QString keySequence = derflaApp->settings_.value("hotkey", "Alt+Space").toString();
#if defined(Q_OS_WIN)
    hotkeyManager_->setKey(QKeySequence(keySequence));
#else
    hotkeyManager_->registerHotkey(keySequence);
#endif
}

void DerflaWidget::onCheckUpdates()
{
    derflaApp->checkForUpdates();
}

void DerflaWidget::onCustomContextMenuRequested(const QPoint &pos)
{
    auto menu = derflaApp->trayIcon_->contextMenu();
    menu->exec(mapToGlobal(pos));
}

void DerflaWidget::showCandidateList()
{
    if (hasFocus() || isActiveWindow() || input_->hasFocus())
    {
        candidateList_->move(mapToGlobal(QPoint(input_->x(), input_->y() + input_->height())));
        
        QString inputText = input_->text().trimmed();
    
        candidateList_->update(inputText);        
    }
}

void DerflaWidget::processKey()
{
    //qDebug() << "DerflaWidget::processKey";
}

void DerflaWidget::doEnter()
{
    qDebug() << "DerflaWidget::doEnter";
    if (candidateList_->isVisible())
        candidateList_->onEnter();
}

void DerflaWidget::doTab()
{
    //qDebug() << "DerflaWidget::doTab";
}

void DerflaWidget::doBackTab()
{
    //qDebug() << "DerflaWidget::doBackTab";
}

bool DerflaWidget::applySkin(const QString& skin)
{
    if (!skinManager_->applySkin(skin))
        return false;

    resize(1, 1);
    resize(skinManager_->skinSize());

    input_->setStyleSheet(skinManager_->inputStyle());
    QFont f = input_->font();
    f.setFamily(globalDefaultFontFamily);
    input_->setFont(f);

    candidateList_->setInputBoxSize(input_->size());
    
    return true;
}

void DerflaWidget::hideCandidateList()
{
    if (candidateList_->isVisible())
        candidateList_->hide();
}

void DerflaWidget::stopWaiting()
{
    QList<QAction*> actions = input_->actions();
    if (actions.isEmpty())
        return;
    QAction* logoAction = actions.at(0);
    logoAction->setIcon(QIcon(":/derfla.ico"));
}
