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
#include "CharLineEdit.h"
#include "derflawidget.h"

DerflaWidget::DerflaWidget(QWidget *parent) :
    QWidget(parent)
{
#if defined(Q_OS_MAC)
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
#elif defined(Q_OS_WIN)
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
#endif
    setAttribute(Qt::WA_InputMethodEnabled);
    setAttribute(Qt::WA_TranslucentBackground);

    setFocusPolicy(Qt::ClickFocus);

    QString s = QApplication::applicationDirPath();
#if defined(Q_OS_MAC)
    QDir d(s);
    d.cdUp();
    d.cd("Resources");
    s = d.absolutePath() + "/skins/derfla.png";
#else
    s = s + "/skins/derfla.png";
#endif
    if (!pic.load(s))
        qDebug() << "can't load picture from " << s;
    resize(pic.size());
    mouseMovePos = QPoint(0, 0);
//    timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(repaint()));
//    timer->start(100);

    input = new CharLineEdit(this);
    input->move(56,96);
#ifdef Q_WS_MAC
    QMacStyle::setFocusRectPolicy(input, QMacStyle::FocusDisabled);
#endif
    input->setObjectName("input");
    connect(input, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(inputKeyPressEvent(QKeyEvent*)));
    connect(input, SIGNAL(focusIn(QFocusEvent*)), this, SLOT(focusInEvent(QFocusEvent*)));
    connect(input, SIGNAL(focusOut(QFocusEvent*)), this, SLOT(focusOutEvent(QFocusEvent*)));
    connect(input, SIGNAL(inputMethod(QInputMethodEvent*)), this, SLOT(inputMethodEvent(QInputMethodEvent*)));


    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    addAction(quitAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);
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
    painter.drawPixmap(0, 0, pic);
    painter.setRenderHint(QPainter::Antialiasing);
//    painter.drawText(56, 76, QDateTime::currentDateTime().toString("hh:mm:ss"));
}

void DerflaWidget::focusInEvent(QFocusEvent *event)
{
//    if (event->gotFocus() && fader->isFading())
//        fader->fadeIn(false);


    QWidget::focusInEvent(event);
}

void DerflaWidget::focusOutEvent(QFocusEvent *event)
{
//    if (event->reason() == Qt::ActiveWindowFocusReason)
//    {
//        if (gSettings->value("GenOps/hideiflostfocus", false).toBool() &&
//                !isActiveWindow() && !alternatives->isActiveWindow() && !optionsOpen && !fader->isFading())
//        {
//            hideLaunchy();
//        }
//    }
}

void DerflaWidget::inputMethodEvent(QInputMethodEvent *event)
{
    processKey();
}

void DerflaWidget::inputKeyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Tab)
    {
        keyPressEvent(event);
    }
    else
    {
        event->ignore();
    }
}

void DerflaWidget::processKey()
{

}
