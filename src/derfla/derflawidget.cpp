#include "stdafx.h"
#include <private/qzipreader_p.h>
#include "uglobalhotkeys.h"
#include "charlineedit.h"
#include "extensionmanager.h"
#include "candidatelist.h"
#include "derflawidget.h"

DerflaWidget::DerflaWidget(QWidget *parent)
    : QWidget(parent)
    , mouseMovePos_(0, 0)
    , candidateDelayTimer_(new QTimer(this))
    , input_(new CharLineEdit(this))
    , extensionManager_(new ExtensionManager(this))
    , candidateList_(new CandidateList(extensionManager_))
    , hotkeyManager_(new UGlobalHotkeys(this))
{
#if defined(Q_OS_WIN)
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool );
#else
    setWindowFlags(Qt::FramelessWindowHint );
#endif
    setAttribute(Qt::WA_InputMethodEnabled);
    setAttribute(Qt::WA_TranslucentBackground);

    setFocusPolicy(Qt::ClickFocus);

    QSettings settings;

    stayOnTop_ = settings.value("stayOnTop", false).toBool();

    QString skinPath = settings.value("skin", ":/skins/derfla.derflaskin").toString();
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

    extensionManager_->loadAllFromLocal();
    extensionManager_->loadAllFromCache();
    connect(candidateList_, &CandidateList::done, this, &DerflaWidget::candidateListDone);
    connect(candidateList_, &CandidateList::keyPressedEvent, this, &DerflaWidget::keyPressed);

//    QAction *logoAction = new QAction(tr("Input"), this);
//    logoAction->setIcon(QIcon(":/derfla.ico"));
//    input_->addAction(logoAction, QLineEdit::ActionPosition::TrailingPosition);
    input_->setObjectName("input");
    input_->setClearButtonEnabled(false);
    connect(input_, &CharLineEdit::keyPressed, this, &DerflaWidget::keyPressEvent);
    connect(input_, &QLineEdit::textChanged, this, &DerflaWidget::inputChanged);

    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, &QAction::triggered, this, &DerflaWidget::quit);
    addAction(quitAction);

    QAction *loadSkinAction = new QAction(tr("Load &Skin"), this);
    connect(loadSkinAction, &QAction::triggered, this, &DerflaWidget::loadSkin);
    addAction(loadSkinAction);

    QAction *installExtensionAction = new QAction(tr("&Install Extension"), this);
	connect(installExtensionAction, &QAction::triggered, this, &DerflaWidget::installExtension);
	addAction(installExtensionAction);

    QAction *stayOnTopAction = new QAction(tr("Stay On Top"), this);
    stayOnTopAction->setCheckable(true);
    if (stayOnTop_)
    {
        stayOnTopAction->setChecked(true);
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    }
    connect(stayOnTopAction, &QAction::triggered, this, &DerflaWidget::stayOnTop);
    addAction(stayOnTopAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);

    QAction *showAction = new QAction(tr("Show"), this);
#if defined(Q_OS_WIN)
    showAction->setShortcut(tr("Alt+Space"));
#else
    showAction->setShortcut(tr("Ctrl+Alt+Space"));
#endif
    connect(showAction, &QAction::triggered, this, &DerflaWidget::showInFront);

    QMenu* trayiconMenu = new QMenu(this);
    trayiconMenu->addAction(showAction);
    trayiconMenu->addAction(loadSkinAction);
    trayiconMenu->addAction(installExtensionAction);
    trayiconMenu->addAction(stayOnTopAction);
    trayiconMenu->addAction(quitAction);
    trayIcon_ = new QSystemTrayIcon(this);
    connect(trayIcon_, &QSystemTrayIcon::activated, this, &DerflaWidget::trayIconActivated);
    trayIcon_->setContextMenu(trayiconMenu);
    trayIcon_->setIcon(QIcon(":/derfla.ico"));
    trayIcon_->setToolTip(tr("Derfla - Accelerate your keyboard!"));
    trayIcon_->show();

    connect(candidateDelayTimer_, &QTimer::timeout, this, &DerflaWidget::onCandidateDelayTimer);
    candidateDelayTimer_->setSingleShot(true);
#if defined(Q_OS_WIN)
    hotkeyManager_->registerHotkey("Alt+Space");
#else
    hotkeyManager_->registerHotkey("Ctrl+Alt+Space");
#endif
    connect(hotkeyManager_, &UGlobalHotkeys::activated, this,  &DerflaWidget::showInFront);
}

DerflaWidget::~DerflaWidget()
{
    candidateDelayTimer_->stop();
    hotkeyManager_->unregisterHotkey();
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
    QSize size(backgroundImage_.size());

    if (size.width() > widgetMinWidth_)
        painter.drawPixmap(0, 0, backgroundImage_);
    else
    {
        painter.drawPixmap(0, 0, leftPartBackgroundImage_);
        painter.drawPixmap(leftPartBackgroundImage_.width(), 0, midPartBackgroundImage_);
        painter.drawPixmap(leftPartBackgroundImage_.width() + midPartBackgroundImage_.width(), 0, rightPartBackgroundImage_);
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

void DerflaWidget::inputChanged(const QString &text)
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
#if defined(Q_OS_WIN)
        candidateDelayTimer_->start(200);
#else
        candidateDelayTimer_->start(100);
#endif
    }
}

void DerflaWidget::keyPressed(QKeyEvent *e)
{

    //qDebug() << "DerflaWidget::keyPressed" << e;
    if ( e->key() != Qt::Key_Escape)
        hideCandidateList();
    activateWindow();
    raise();
    qApp->sendEvent(this, e);
}

void DerflaWidget::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
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

void DerflaWidget::loadSkin()
{    
    hideCandidateList();
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Load Derfla Skin"),
        "",
        tr("Derfla Skin Package (*.derflaskin);;Derfla Skin Configuration (skin.xml);;All files (*.*)"));
    if (!QFile::exists(fileName))
        return;
    if (applySkin(fileName))
    {
        QSettings settings;
        settings.setValue("skin", fileName);
        settings.sync();
    }
}

void DerflaWidget::installExtension()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Install Derfla Extension"),
		"",
        tr("Derfla Extension Package (*.derflaextension);;Derfla Extension Configuration (extension.cfg);;All files (*.*)"));
    if (!QFile::exists(fileName))
        return;
    try {
        extensionManager_->installExtension(fileName);
    }
    catch(std::runtime_error& e) {
        QMessageBox::warning(this, tr("Installing extension failed"), QString::fromLatin1(e.what()), QMessageBox::Ok);
    }
}

void DerflaWidget::stayOnTop()
{
    QAction *action = qobject_cast<QAction*>(sender());
    stayOnTop_ = !stayOnTop_;
    action->setChecked(stayOnTop_);
    if (stayOnTop_)
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    else
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    showInFront();

    QSettings settings;
    settings.setValue("stayOnTop", stayOnTop_);
    settings.sync();
}

void DerflaWidget::showInFront()
{
    if (isHidden())
        show();
    activateWindow();
    raise();
}

void DerflaWidget::candidateListDone()
{
    hideCandidateList();
    input_->setText("");
}

void DerflaWidget::quit()
{
    qApp->quit();
}

void DerflaWidget::onCandidateDelayTimer()
{
    showCandidateList();
}

void DerflaWidget::showCandidateList()
{
    //candidateList_->show();
    candidateList_->move(mapToGlobal(QPoint(input_->x(), input_->y() + input_->height())));
    
    QString inputText = input_->text();

    candidateList_->update(inputText);
}

void DerflaWidget::processKey()
{
    //qDebug() << "DerflaWidget::processKey";
}

void DerflaWidget::doEnter()
{
    //qDebug() << "DerflaWidget::doEnter";
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
    QString s;
    if (!QFileInfo::exists(skin))
    {
        if (applySkin(QString(":/skins/%1.derflaskin").arg(skin)))
            return true;
        // load by skin name 
        s = QApplication::applicationDirPath();
        const QString skinPath = QString("/skins/%1.xml").arg(skin);
#if defined(Q_OS_MAC)
        QDir d(s);
        d.cdUp();
        d.cd("Resources");
        s = d.absolutePath() + skinPath;
#else
        s += skinPath;
#endif
        if (!QFile::exists(s))
        {
            // load by skin package - *.derflaskin, should be decompressed first
            int index = s.lastIndexOf(".xml");
            Q_ASSERT(index > 0);
            s.remove(index, 4);
            s.append(".derflaskin");
            if (!loadSkinPackage(s, s))
            {
                return false;
            }
        }
    }
    else
    {
        QFileInfo fi(skin);
        if (fi.suffix() == "xml")
        {
            // load by skin configuration file - *.xml
            s = skin;
        }
        else
        {
            // load by skin package - *.derflaskin, should be decompressed first
            if (!loadSkinPackage(skin, s))
            {
                return false;
            }
        }
    }

    QString imagePath;
    QString inputStyle;
    int cutTop = -1, cutBottom = -1;
    if (!loadSkinConfiguration(s, imagePath, inputStyle, cutTop, cutBottom))
    {
        return false;
    }

    if (!backgroundImage_.load(imagePath))
    {
        qCritical() << "can't load picture from " << imagePath;
        return false;
    }

    QSize size = backgroundImage_.size();

    if (cutTop >= 0 && cutBottom > cutTop)
    {
        QPixmap topPartBackgroundImage = backgroundImage_.copy(0, 0, size.width(), cutTop);
        QPixmap cutPartBackgroundImage = backgroundImage_.copy(0, cutTop, size.width(), cutBottom - cutTop);
        QPixmap bottomPartBackgroundImage = backgroundImage_.copy(0, cutBottom, size.width(), size.height() - cutBottom);
        size.setHeight(size.height() - (cutBottom - cutTop));
        qDebug() << topPartBackgroundImage.size() << cutPartBackgroundImage.size() << bottomPartBackgroundImage.size() << size << backgroundImage_.size();
        QPixmap t(size);
        t.fill(Qt::transparent);
        QPainter painter(&t);
        painter.drawPixmap(0, 0, size.width(), cutTop, topPartBackgroundImage);
        painter.drawPixmap(0, cutTop, size.width(), size.height()- cutTop, bottomPartBackgroundImage);
        backgroundImage_ = t.copy(0, 0, size.width(), size.height());
    }

    if (size.width() < widgetMinWidth_)
    {
        leftPartBackgroundImage_ = backgroundImage_.copy(0, 0,
                                                         size.width() / 2 -1, size.height());
        midPartBackgroundImage_ = backgroundImage_.copy(size.width() / 2 - 1, 0,
                                                        2, size.height()).scaled(widgetMinWidth_ - (size.width() - 2), size.height());
        rightPartBackgroundImage_ = backgroundImage_.copy(size.width() / 2 + 1, 0,
                                                          size.width() / 2 - 1, size.height());

        size.setWidth(widgetMinWidth_);
    }
    resize(1, 1);
    resize(size);

    input_->setStyleSheet(inputStyle);
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

bool DerflaWidget::loadSkinConfiguration(const QString& configurationPath, QString& bgImagePath, QString& inputStyle, int& cutTop, int& cutBottom)
{
    QDomDocument doc;
    QFile file(configurationPath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qCritical() << "can't open skin configuration file" << configurationPath;
        return false;
    }

    if (!doc.setContent(&file))
    {
        qCritical() << "can't parse skin configuration file" << configurationPath;
        file.close();
        return false;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    QDomElement imageElem = docElem.firstChildElement("image");
    if (imageElem.isNull())
    {
        qCritical() << "missing image element in skin configuration file" << configurationPath;
        return false;
    }
    
    QFileInfo cfg(configurationPath);

    bgImagePath = QString("%1/%2").arg(cfg.absolutePath()).arg(imageElem.text());

    QDomElement issElem = docElem.firstChildElement("inputstyle");
    if (issElem.isNull())
    {
        qCritical() << "missing inputstyle element in skin configuration file" << configurationPath;
        return false;
    }
    inputStyle = issElem.text();

    QDomElement cutTopElem = docElem.firstChildElement("cuttop");
    if (!cutTopElem.isNull())
    {
        cutTop = cutTopElem.text().toInt();
    }
    QDomElement cutBottomElem = docElem.firstChildElement("cutbottom");
    if (!cutBottomElem.isNull())
    {
        cutBottom = cutBottomElem.text().toInt();
    }

    return true;
}

bool DerflaWidget::loadSkinPackage(const QString& skinPath, QString& configurationPath)
{
    QString dirName = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) % "/SkinTmp";
    QDir dir(dirName);
    dir.removeRecursively();
    dir.mkpath(dirName);
    QZipReader zr(skinPath);
    bool res = zr.extractAll(dirName);
    if (!res)
    {
        qCritical() << "extracting" << skinPath << "to" << dirName << "failed";
        return false;
    }
    configurationPath = dirName % "/skin.xml";
    if (QFile::exists(configurationPath))
        return true;

    configurationPath = dirName % "/" % QFileInfo(skinPath).completeBaseName() % ".xml";
    if (QFile::exists(configurationPath))
        return true;

    configurationPath.clear();
    qCritical() << "can't find configuration file in skin package" << skinPath;
    return false;
}

void DerflaWidget::stopWaiting()
{
    QList<QAction*> actions = input_->actions();
    if (actions.isEmpty())
        return;
    QAction* logoAction = actions.at(0);
    logoAction->setIcon(QIcon(":/derfla.ico"));
}
