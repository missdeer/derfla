#include "stdafx.h"
#include <JlCompress.h>
#include "uglobalhotkeys.h"
#include "CharLineEdit.h"
#include "candidatelist.h"
#include "localfsscanner.h"
#include "derflawidget.h"

DerflaWidget::DerflaWidget(QWidget *parent) :
    QWidget(parent),
    mouseMovePos_(0, 0),
    loadingAnimationTimer_(new QTimer(this)),
    input_(new CharLineEdit(this)),
    candidateList_(new CandidateList),
    hotkeyManager_(new UGlobalHotkeys),
    localFSScanner_(new LocalFSScanner)
{
#if defined(Q_OS_WIN)
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool );
#else
    setWindowFlags(Qt::FramelessWindowHint );
#endif
    setAttribute(Qt::WA_InputMethodEnabled);
    setAttribute(Qt::WA_TranslucentBackground);

    setFocusPolicy(Qt::ClickFocus);
    
    if (!applySkin(":/skins/derfla.derflaskin"))
    {
        qCritical() << "loading skin failed";
        return;
    }
#ifdef Q_WS_MAC
    QMacStyle::setFocusRectPolicy(input, QMacStyle::FocusDisabled);
#endif

    connect(candidateList_, &CandidateList::done, this, &DerflaWidget::candidateListDone);
    connect(candidateList_, &CandidateList::keyPressedEvent, this, &DerflaWidget::keyPressed);

    QAction *logoAction = new QAction(tr("Input"), this);
    logoAction->setIcon(QIcon(":/derfla.ico"));
    input_->addAction(logoAction, QLineEdit::ActionPosition::TrailingPosition);
    input_->setObjectName("input");
    input_->setClearButtonEnabled(false);
    connect(input_, &CharLineEdit::keyPressed, this, &DerflaWidget::keyPressEvent);
    connect(input_, &QLineEdit::textChanged, this, &DerflaWidget::inputChanged);

    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, &QAction::triggered, this, &DerflaWidget::quit);
    addAction(quitAction);

    QAction *clearAction = new QAction(tr("&Clear Input"), this);
    clearAction->setShortcut(tr("Ctrl+U"));
    connect(clearAction, &QAction::triggered, input_, &QLineEdit::clear);
    addAction(clearAction);

    QAction *loadSkinAction = new QAction(tr("Load &Skin"), this);
    loadSkinAction->setShortcut(tr("Ctrl+O"));
    connect(loadSkinAction, &QAction::triggered, this, &DerflaWidget::loadSkin);
    addAction(loadSkinAction);

    QAction *installAlfredWorkflow = new QAction(tr("Install &Alfred Workflow"), this);
    installAlfredWorkflow->setShortcut(tr("Ctrl+I"));
    connect(installAlfredWorkflow, &QAction::triggered, this, &DerflaWidget::installAlfredWorkflows);
    addAction(installAlfredWorkflow);

    setContextMenuPolicy(Qt::ActionsContextMenu);

    QAction *showAction = new QAction(tr("Show"), this);
    showAction->setShortcut(tr("Alt+Space"));
    connect(showAction, &QAction::triggered, this, &DerflaWidget::showInFront);

    QMenu* trayiconMenu = new QMenu(this);
    trayiconMenu->addAction(showAction);
    trayiconMenu->addAction(loadSkinAction);
    trayiconMenu->addAction(installAlfredWorkflow);
    trayiconMenu->addAction(quitAction);
    trayIcon_ = new QSystemTrayIcon(this);
    connect(trayIcon_, &QSystemTrayIcon::activated, this, &DerflaWidget::trayIconActivated);
    trayIcon_->setContextMenu(trayiconMenu);
    trayIcon_->setIcon(QIcon(":/derfla.ico"));
    trayIcon_->setToolTip(tr("Derfla - Accelerate your keyboard!"));
    trayIcon_->show();

    connect(loadingAnimationTimer_, SIGNAL(timeout()), this, SLOT(onLoadingAnimationTimer()));

    hotkeyManager_->registerHotkey("Alt+Space");
    connect(hotkeyManager_, &UGlobalHotkeys::activated, this,  &DerflaWidget::showInFront);

    connect(localFSScanner_, &LocalFSScanner::finished, this, &DerflaWidget::finishedScan);
    localFSScanner_->start();
    QTimer::singleShot(60 * 60 * 1000, this, &DerflaWidget::scheduleScan);

    QTimer::singleShot(1000, this, &DerflaWidget::loadInstalledAlfredWorkflows);
}

DerflaWidget::~DerflaWidget()
{
    delete localFSScanner_;
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
        painter.drawPixmap(size.width() / 2 - 1, 0, midPartBackgroundImage_);
        painter.drawPixmap(widgetMinWidth_ - (size.width() / 2 - 1), 0, rightPartBackgroundImage_);
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
    check_expiration;
    //qDebug() << "DerflaWidget::keyPressEvent 0:" << event->key();
    if (event->key() == Qt::Key_Escape)
    {
        //qDebug() << "DerflaWidget::keyPressEvent escape" << candidateList_->isVisible();
        if (candidateList_->isVisible())
            candidateList_->hide();
        else
        {
            if (input_->text().isEmpty())
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
    check_expiration;
    //qDebug() <<  "DerflaWidget::inputChanged:" << input_->text();
    if (text.isEmpty())
    {
        hideCandidateList();
        stopWaiting();
    }
    else
    {
        showCandidateList();
        waiting();
    }
}

void DerflaWidget::keyPressed(QKeyEvent *e)
{
    check_expiration;
    //qDebug() << "DerflaWidget::keyPressed" << e;
    if ( e->key() != Qt::Key_Escape)
        hideCandidateList();
    activateWindow();
    raise();
    qApp->sendEvent(this, e);
}

void DerflaWidget::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    check_expiration;
    switch(reason)
    {
    case QSystemTrayIcon::DoubleClick:
        if (!isVisible())
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
    check_expiration;
    hideCandidateList();
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Load Derfla Skin"),
        "",
        tr("Derfla Skin File (*.derflaskin);;Derfla Skin Configuration (*.xml);;All files (*.*)"));
    if (fileName.isEmpty())
        return;
    applySkin(fileName);
}

void DerflaWidget::onLoadingAnimationTimer()
{
    //static int degree = 0;
    //degree += 36;
    //if (degree > 360)
    //    degree -= 360;
    //QList<QAction*> actions = input_->actions();
    //if (actions.isEmpty())
    //    return;
    //QAction* logoAction = actions.at(0);
    //QPixmap icon(":/loading.png");
    //QMatrix rm;
    //rm.rotate(degree);
    //icon = icon.transformed(rm);
    //logoAction->setIcon(icon);
}

void DerflaWidget::showInFront()
{
    check_expiration;
    if (!isVisible())
        show();
    activateWindow();
    raise();
}

void DerflaWidget::candidateListDone()
{
    check_expiration;
    hideCandidateList();
    input_->setText("");
}

void DerflaWidget::quit()
{
    localFSScanner_->stop();
    qApp->quit();
}

void DerflaWidget::installAlfredWorkflows()
{
    check_expiration;
    hideCandidateList();
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
#if defined(Q_OS_MAC)
        tr("Install Alfred Workflow"),
#else
        tr("Install Alfred Workflow (Notice: Some system dependent work flow may not work properly on this system.)"),
#endif
        "",
        tr("Alfred Workflow (*.alfredworkflow)"));
    if (fileNames.isEmpty())
        return;

    std::for_each(fileNames.begin(), fileNames.end(),
                  [&](const QString& path) {
        AlfredWorkflowPtr aw(new AlfredWorkflow);
        if (aw->installFromBundle(path))
        {
            auto it = std::find_if(alfredWorkflowList_.begin(), alfredWorkflowList_.end(),
                                   [&](AlfredWorkflowPtr a) { return aw->bundleId() == a->bundleId(); });
            if (alfredWorkflowList_.end() != it)
            {
                qWarning() << "found an existed workflow with the same bundle id" << aw->bundleId();
                //alfredWorkflowList_.erase(it);
            }
            alfredWorkflowList_.append(aw);
        }
    });
}

void DerflaWidget::scheduleScan()
{
    if (qApp->activeWindow())
    {
        qDebug() << "since windows is active, try in 5 minutes" << QThread::currentThreadId();
        QTimer::singleShot(5 * 60 * 1000, this, &DerflaWidget::scheduleScan);
    }
    else
    {
        qDebug() << "schedule scan now" << QThread::currentThreadId();
        localFSScanner_->start();
        QTimer::singleShot(60 * 60 * 1000, this, &DerflaWidget::scheduleScan);
    }
}

void DerflaWidget::finishedScan()
{
    qDebug() << "stop scan local file system" << QThread::currentThreadId();
    localFSScanner_->stop();
}

void DerflaWidget::showCandidateList()
{
    check_expiration;
    candidateList_->show();
    candidateList_->move(mapToGlobal(QPoint(input_->x(), input_->y() + input_->height())));
    
    QString inputText = input_->text();

    AlfredWorkflowList alfredWorkflowList;
    std::copy_if(alfredWorkflowList_.begin(), alfredWorkflowList_.end(), std::back_inserter(alfredWorkflowList), 
        [&inputText](AlfredWorkflowPtr aw) {
        return aw->hitKeyword(inputText);
    });

    if (alfredWorkflowList.empty())
        candidateList_->update(inputText);
    else
        candidateList_->update(alfredWorkflowList, inputText);
}

void DerflaWidget::processKey()
{
    check_expiration;
    //qDebug() << "DerflaWidget::processKey";
}

void DerflaWidget::doEnter()
{
    check_expiration;
    //qDebug() << "DerflaWidget::doEnter";
    candidateList_->onEnter();
}

void DerflaWidget::doTab()
{
    check_expiration;
    //qDebug() << "DerflaWidget::doTab";
}

void DerflaWidget::doBackTab()
{
    check_expiration;
    //qDebug() << "DerflaWidget::doBackTab";
}

bool DerflaWidget::applySkin(const QString& skin)
{
    check_expiration;
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
    
    return true;
}

void DerflaWidget::waiting()
{
    check_expiration;
    loadingAnimationTimer_->start(100);
}

void DerflaWidget::hideCandidateList()
{
    check_expiration;
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
    QStringList files = JlCompress::extractDir(skinPath, dirName);
    if (files.empty())
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

void DerflaWidget::loadInstalledAlfredWorkflows()
{
    QString dirName = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) % "/alfredworkflow";
    QDir dir(dirName);
    if (!dir.exists())
    {
        qWarning() << dirName << "doesn't exist";
        return;
    }

    QFileInfoList list = dir.entryInfoList(QStringList() << "*", QDir::NoDotAndDotDot | QDir::AllDirs);
    std::for_each(list.begin(), list.end(), [&](const QFileInfo& fi) {
        QString absolutePath = dirName % "/" % fi.fileName();
        AlfredWorkflowPtr aw(new AlfredWorkflow);
        if (aw->loadFromDirectory(absolutePath))
        {
            auto it = std::find_if(alfredWorkflowList_.begin(), alfredWorkflowList_.end(),
                [&](AlfredWorkflowPtr a) { return aw->bundleId() == a->bundleId(); });
            if (alfredWorkflowList_.end() != it)
            {
                qWarning() << "found an existed workflow with the same bundle id" << aw->bundleId();
                //alfredWorkflowList_.erase(it);
            }
            alfredWorkflowList_.append(aw);
        }
    });
}

void DerflaWidget::stopWaiting()
{
    check_expiration;
    loadingAnimationTimer_->stop();

    QList<QAction*> actions = input_->actions();
    if (actions.isEmpty())
        return;
    QAction* logoAction = actions.at(0);
    logoAction->setIcon(QIcon(":/derfla.ico"));
}
