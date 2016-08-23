#include "stdafx.h"
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
    candidateList_(nullptr),
    hotkeyManager_(new UGlobalHotkeys),
    localFSScanner_(new LocalFSScanner())
{
#if defined(Q_OS_MAC)
    setWindowFlags(Qt::FramelessWindowHint );
#elif defined(Q_OS_WIN)
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool );
#endif
    setAttribute(Qt::WA_InputMethodEnabled);
    setAttribute(Qt::WA_TranslucentBackground);

    setFocusPolicy(Qt::ClickFocus);


    if (!applySkin("derfla"))
    {
        //qDebug() << "loading skin failed";
        return;
    }
#ifdef Q_WS_MAC
    QMacStyle::setFocusRectPolicy(input, QMacStyle::FocusDisabled);
#endif
    QAction *logoAction = new QAction(tr("Input"), this);
    logoAction->setIcon(QIcon(":/derfla.ico"));
    input_->addAction(logoAction, QLineEdit::ActionPosition::TrailingPosition);
    input_->setObjectName("input");
    input_->setClearButtonEnabled(false);
    connect(input_, &CharLineEdit::keyPressed, this, &DerflaWidget::keyPressEvent);
    connect(input_, &QLineEdit::textChanged, this, &DerflaWidget::inputChanged);

    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    addAction(quitAction);

    QAction *clearAction = new QAction(tr("&Clear Input"), this);
    clearAction->setShortcut(tr("Ctrl+U"));
    connect(clearAction, SIGNAL(triggered()), input_, SLOT(clear()));
    addAction(clearAction);

    QAction *loadSkinAction = new QAction(tr("Load &Skin"), this);
    loadSkinAction->setShortcut(tr("Ctrl+O"));
    connect(loadSkinAction, SIGNAL(triggered()), this, SLOT(loadSkin()));
    addAction(loadSkinAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);

    QAction *showAction = new QAction(tr("Show"), this);
    showAction->setShortcut(tr("Alt+Space"));
    connect(showAction, SIGNAL(triggered()), this, SLOT(showInFront()));

    QMenu* trayiconMenu = new QMenu(this);
    trayiconMenu->addAction(showAction);
    trayiconMenu->addAction(loadSkinAction);
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

    connect(localFSScanner_, &LocalFSScanner::finished, this, &DerflaWidget::localFSScannerFinished);
    localFSScanner_->start();
}

DerflaWidget::~DerflaWidget()
{
    delete localFSScanner_;
    hotkeyManager_->unregisterHotkey();
}

void DerflaWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(mouseMovePos_ != QPoint(0, 0))
    {
        move(geometry().x() + event->globalPos().x() - mouseMovePos_.x(), geometry().y() + event->globalPos().y() - mouseMovePos_.y());
        mouseMovePos_ = event->globalPos();
    }
}

void DerflaWidget::mousePressEvent(QMouseEvent *event)
{
    mouseMovePos_ = event->globalPos();
}

void DerflaWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mouseMovePos_ = QPoint(0, 0);
}

void DerflaWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, backgroundImage_);
    painter.setRenderHint(QPainter::Antialiasing);
}

void DerflaWidget::moveEvent(QMoveEvent *event)
{
    if (candidateList_ && candidateList_->isVisible())
        candidateList_->move(mapToGlobal(QPoint(input_->x(), input_->y() + input_->height())));
}

void DerflaWidget::keyPressEvent(QKeyEvent *event)
{
    check_expiration;
    static QDateTime lastTime = QDateTime::currentDateTime();
    QDateTime now = QDateTime::currentDateTime();
    //qDebug() << "DerflaWidget::keyPressEvent 0:" << event->key() << now;
    if (event->key() == Qt::Key_Escape)
    {
        if (lastTime.msecsTo(now) > 50)
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
    lastTime = now;
}

void DerflaWidget::inputChanged(const QString &text)
{
    check_expiration;
    //qDebug() <<  "DerflaWidget::inputChanged:" << input_->text();
    if (input_->text().isEmpty())
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
    QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Load Derfla Skin"),
                                                        "",
                                                        tr("Derfla Skin Configuration (*.xml);;All files (*.*)"));
    if (fileName.isEmpty())
            return;
    QFileInfo f(fileName);
    applySkin(f.baseName());
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

void DerflaWidget::localFSScannerFinished()
{
    QTimer::singleShot(60 * 60 *1000, this, &DerflaWidget::onNextLFSScanning);
}

void DerflaWidget::onNextLFSScanning()
{
    if (qApp->activeWindow())
        QTimer::singleShot(5 * 60 * 1000, this, &DerflaWidget::onNextLFSScanning);
    else
        QTimer::singleShot(1000, localFSScanner_, &LocalFSScanner::scan);
}

void DerflaWidget::showCandidateList()
{
    check_expiration;
    if (!candidateList_)
    {
        candidateList_ = new CandidateList();
        connect(candidateList_, &CandidateList::done, this, &DerflaWidget::candidateListDone);
        connect(candidateList_, &CandidateList::keyPressedEvent, this, &DerflaWidget::keyPressed);
    }
    candidateList_->show();
    candidateList_->move(mapToGlobal(QPoint(input_->x(), input_->y() + input_->height())));
    candidateList_->update(input_->text());
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
    QString s = QApplication::applicationDirPath();
    const QString skinPath = QString("/skins/%1.xml").arg(skin);
#if defined(Q_OS_MAC)
    QDir d(s);
    d.cdUp();
    d.cd("Resources");
    s = d.absolutePath() +skinPath ;
#else
    s += skinPath;
#endif

    QDomDocument doc;
    QFile file(s);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if (!doc.setContent(&file))
    {
        file.close();
        return false;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    QDomElement imageElem = docElem.firstChildElement("image");
    if (imageElem.isNull())
        return false;

    s = QApplication::applicationDirPath();
#if defined(Q_OS_MAC)
    d.setPath(s);
    d.cdUp();
    d.cd("Resources");
    s = d.absolutePath();
#endif
    QString imagePath = QString("%1/skins/%2").arg(s).arg(imageElem.text());

    if (!backgroundImage_.load(imagePath))
    {
        //qDebug() << "can't load picture from " << imagePath;
        return false;
    }
    resize(backgroundImage_.size());

    QDomElement xElem = docElem.firstChildElement("x");
    if (xElem.isNull())
        return false;
    QDomElement yElem = docElem.firstChildElement("y");
    if (yElem.isNull())
        return false;
    QDomElement wElem = docElem.firstChildElement("width");
    if (wElem.isNull())
        return false;
    QDomElement hElem = docElem.firstChildElement("height");
    if (hElem.isNull())
        return false;

    input_->setGeometry(xElem.text().toInt(), yElem.text().toInt(), wElem.text().toInt(), hElem.text().toInt());
    QFont f = input_->font();
    QDomElement fzElem = docElem.firstChildElement("pixelsize");
    if (fzElem.isNull())
        return false;
    f.setPixelSize(fzElem.text().toInt());
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
