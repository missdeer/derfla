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
#include "CharLineEdit.h"
#include "candidatelist.h"
#include "derflawidget.h"

DerflaWidget::DerflaWidget(QWidget *parent) :
    QWidget(parent),
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
    input->setGeometry(46, 62, 150, 68);
    QFont f = input->font();
    f.setPixelSize(48);
    f.setFamily("Menlo");
    input->setFont(f);
#ifdef Q_WS_MAC
    QMacStyle::setFocusRectPolicy(input, QMacStyle::FocusDisabled);
#endif
    input->setObjectName("input");
    connect(input, &QLineEdit::textChanged, this, &DerflaWidget::inputChanged);

    QAction *quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcut(tr("Ctrl+Q"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    addAction(quitAction);

    QAction *clearAction = new QAction(tr("Clear Input"), this);
    clearAction->setShortcut(tr("Ctrl+U"));
    connect(clearAction, SIGNAL(triggered()), input, SLOT(clear()));
    addAction(clearAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);

    QMenu* trayiconMenu = new QMenu(this);
    trayiconMenu->addAction(quitAction);
    trayicon = new QSystemTrayIcon(this);
    trayicon->setContextMenu(trayiconMenu);
    trayicon->setIcon(QIcon(":/derfla.ico"));
    trayicon->setToolTip(tr("Derfla - Accelerate your keyboard!"));
    trayicon->show();
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

void DerflaWidget::moveEvent(QMoveEvent *event)
{
    if (candidatelist && candidatelist->isVisible())
        candidatelist->move(mapToGlobal(QPoint(input->x(), input->y() + input->height())));
}

void DerflaWidget::inputChanged(const QString &text)
{
    qDebug() << input->text();
    ShowCandidateList();
}

void DerflaWidget::ShowCandidateList()
{
    if (!candidatelist)
        candidatelist = new CandidateList();
    candidatelist->show();
    candidatelist->move(mapToGlobal(QPoint(input->x(), input->y() + input->height())));
    candidatelist->update(input->text());
}
