#include <QtCore>
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QBitmap>
#include <QRect>
#include <QPainter>
#include <QDateTime>
#include "derflawidget.h"

DerflaWidget::DerflaWidget(QWidget *parent) :
    QWidget(parent)
{
#if defined(Q_OS_MAC)
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
#elif defined(Q_OS_WIN)
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
#endif
    setAttribute(Qt::WA_TranslucentBackground);
    QString s = QApplication::applicationDirPath();
#if defined(Q_OS_MAC)
    QDir d(s);
    d.cdUp();
    d.cd("Resources");
    s = d.absolutePath() + "/01.png";
#else
    s = s + "/01.png";
#endif
    if (!pic.load(s))
        qDebug() << "can't load picture";
    resize(pic.size());
    mouseMovePos = QPoint(0, 0);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(repaint()));
    timer->start(100);
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
    painter.drawText(66, 76, QDateTime::currentDateTime().toString("hh:mm:ss"));
}
