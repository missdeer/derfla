#include <QtCore>
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QBitmap>
#include <QRect>
#include <QPainter>
#include <QDateTime>
#include <QAction>
#include <QMenu>
#include <QIcon>
#include <QDomDocument>
#include <QFileDialog>
#include "CharLineEdit.h"
#include "candidatelist.h"
#include "derflawidget.h"

DerflaWidget::DerflaWidget(QWidget *parent) :
    QWidget(parent),
    mouseMovePos(0, 0),
    timer(new QTimer(this)),
    input(new CharLineEdit(this)),
    candidatelist(nullptr)
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
        qDebug() << "loading skin failed";
        return;
    }
#ifdef Q_WS_MAC
    QMacStyle::setFocusRectPolicy(input, QMacStyle::FocusDisabled);
#endif
    QAction *logoAction = new QAction(tr("Input"), this);
    logoAction->setIcon(QIcon(":/derfla.ico"));
    input->addAction(logoAction, QLineEdit::ActionPosition::TrailingPosition);
    input->setObjectName("input");
    input->setClearButtonEnabled(false);
    connect(input, &CharLineEdit::keyPressed, this, &DerflaWidget::keyPressEvent);
    connect(input, &QLineEdit::textChanged, this, &DerflaWidget::inputChanged);

    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    addAction(quitAction);

    QAction *clearAction = new QAction(tr("&Clear Input"), this);
    clearAction->setShortcut(tr("Ctrl+U"));
    connect(clearAction, SIGNAL(triggered()), input, SLOT(clear()));
    addAction(clearAction);

    QAction *loadSkinAction = new QAction(tr("Load &Skin"), this);
    connect(loadSkinAction, SIGNAL(triggered()), this, SLOT(loadSkin()));
    addAction(loadSkinAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);

    QAction *showAction = new QAction(tr("Show"), this);
    connect(showAction, SIGNAL(triggered()), this, SLOT(showInFront()));

    QMenu* trayiconMenu = new QMenu(this);
    trayiconMenu->addAction(showAction);
    trayiconMenu->addAction(loadSkinAction);
    trayiconMenu->addAction(quitAction);
    trayicon = new QSystemTrayIcon(this);
    connect(trayicon, &QSystemTrayIcon::activated, this, &DerflaWidget::trayIconActivated);
    trayicon->setContextMenu(trayiconMenu);
    trayicon->setIcon(QIcon(":/derfla.ico"));
    trayicon->setToolTip(tr("Derfla - Accelerate your keyboard!"));
    trayicon->show();

    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

DerflaWidget::~DerflaWidget()
{

}

void DerflaWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(mouseMovePos != QPoint(0, 0))
    {
        move(geometry().x() + event->globalPos().x() - mouseMovePos.x(), geometry().y() + event->globalPos().y() - mouseMovePos.y());
        mouseMovePos = event->globalPos();
    }
}

void DerflaWidget::mousePressEvent(QMouseEvent *event)
{
    mouseMovePos = event->globalPos();
}

void DerflaWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mouseMovePos = QPoint(0, 0);
}

void DerflaWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, backgroundImage);
    painter.setRenderHint(QPainter::Antialiasing);
    //    painter.drawText(56, 76, QDateTime::currentDateTime().toString("hh:mm:ss"));
}

void DerflaWidget::moveEvent(QMoveEvent *event)
{
    if (candidatelist && candidatelist->isVisible())
        candidatelist->move(mapToGlobal(QPoint(input->x(), input->y() + input->height())));
}

void DerflaWidget::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "0:" << event->key();
    static QDateTime lastTime = QDateTime::currentDateTime();
    QDateTime now = QDateTime::currentDateTime();
    if (event->key() == Qt::Key_Escape)
    {        
        if (candidatelist->isVisible())
            candidatelist->hide();
        else
        {
            if (lastTime.msecsTo(now) > 10)
            {
                if (input->text().isEmpty())
                    hide();
                else
                    input->setText("");
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
        qDebug() << "1:" << event->key();
        if (candidatelist->isVisible())
        {
            //if (!candidatelist->isActiveWindow())
            {
                // Don't refactor the activateWindow outside the if, it won't work properly any other way!
//                if (candidatelist->currentRow() < 0 && candidatelist->count() > 0)
//                {
//                    candidatelist->activateWindow();
//                    candidatelist->setCurrentRow(0);
//                }
//                else
                {
                    qDebug() << "2:" << event->key();
                    candidatelist->activateWindow();
                    candidatelist->setActiveWindowFlag(true);
                    qApp->sendEvent(candidatelist, event);
                }
            }
        }
        else if (event->key() == Qt::Key_Down || event->key() == Qt::Key_PageDown)
        {
            // do a search and show the results, selecting the first one
//            searchOnInput();
//            if (searchResults.count() > 0)
//            {
//                updatecandidatelist();
//                showcandidatelist();
//            }
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
        if (!input->hasFocus())
        {
            input->setFocus();
            qApp->sendEvent(input, event);
        }
        // process any other key with character output
        event->ignore();
        processKey();
    }
    lastTime = now;
}

void DerflaWidget::inputChanged(const QString &text)
{
    qDebug() << input->text();
    ShowCandidateList();
    if (input->text().isEmpty())
        stopWaiting();
    else
        waiting();
}

void DerflaWidget::keyPressed(QKeyEvent *e)
{
    keyPressEvent(e);
}

void DerflaWidget::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
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
    QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Load Derfla Skin"),
                                                        "",
                                                        tr("Derfla Skin Configuration (*.xml);;All files (*.*)"));
    if (fileName.isEmpty())
            return;
    QFileInfo f(fileName);
    applySkin(f.baseName());
}

void DerflaWidget::onTimer()
{
    static int degree = 0;
    degree += 36;
    if (degree > 360)
        degree -= 360;
    QList<QAction*> actions = input->actions();
    if (actions.isEmpty())
        return;
    QAction* logoAction = actions.at(0);
    QPixmap icon(":/loading.png");
    QMatrix rm;
    rm.rotate(degree);
    icon = icon.transformed(rm);
    logoAction->setIcon(icon);
}

void DerflaWidget::showInFront()
{
    if (!isVisible())
        show();
    activateWindow();
    raise();
}

void DerflaWidget::ShowCandidateList()
{
    if (!candidatelist)
        candidatelist = new CandidateList();
    candidatelist->show();
    candidatelist->move(mapToGlobal(QPoint(input->x(), input->y() + input->height())));
    candidatelist->update(input->text());
}

void DerflaWidget::processKey()
{
    qDebug() << __FUNCTION__;
}

void DerflaWidget::doEnter()
{
    qDebug() << __FUNCTION__;
    candidatelist->onEnter();
}

void DerflaWidget::doTab()
{
    qDebug() << __FUNCTION__;
}

void DerflaWidget::doBackTab()
{
    qDebug() << __FUNCTION__;
}

bool DerflaWidget::applySkin(const QString& skin)
{
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

    if (!backgroundImage.load(imagePath))
    {
        qDebug() << "can't load picture from " << imagePath;
        return false;
    }
    resize(backgroundImage.size());

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

    input->setGeometry(xElem.text().toInt(), yElem.text().toInt(), wElem.text().toInt(), hElem.text().toInt());
    QFont f = input->font();
    QDomElement fzElem = docElem.firstChildElement("pixelsize");
    if (fzElem.isNull())
        return false;
    f.setPixelSize(fzElem.text().toInt());
#if defined(Q_OS_WIN)
    const QString fontFamily = "Microsoft YaHei";
#elif defined(Q_OS_MAC)
    const QString fontFamily = "PingFangCS";
#else
    const QString fontFamily = "Menlo";
#endif
    f.setFamily(fontFamily);
    input->setFont(f);

    return true;
}

void DerflaWidget::waiting()
{
    timer->start(100);
}

void DerflaWidget::stopWaiting()
{
    timer->stop();

    QList<QAction*> actions = input->actions();
    if (actions.isEmpty())
        return;
    QAction* logoAction = actions.at(0);
    logoAction->setIcon(QIcon(":/derfla.ico"));
}
