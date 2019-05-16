#include "doublelistitem.h"
#include "ui_doublelistitem.h"
#include "config.h"
#include <QStringList>
#include <QString>

DoubleListItem::DoubleListItem(const QString& icon, const QString& text, const QString& subtext, const QString& cmd, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DoubleListItem)
{
    ui->setupUi(this);
    ui->subtext->setStyleSheet("QLabel{color: rgb(100, 100, 100)}");
    ui->cmd->setStyleSheet("QLabel{color: rgb(100, 100, 100)}");
    QFontMetrics fm(QFont(ui->text->font()));
    ui->text->setText(fm.elidedText(text, Qt::ElideRight, ui->text->width()));
    QFontMetrics subfm(QFont(ui->subtext->font()));
    ui->subtext->setText(subfm.elidedText(subtext, Qt::ElideRight, ui->subtext->width()));
    ui->cmd->setText(cmd);
    QIcon tmp;
    if (icon.indexOf('/') == -1)
    {
        tmp = QIcon::fromTheme(icon);
    }
    else
    {
        tmp = QIcon(icon);
    }
    if (tmp.isNull())
    {
        tmp = QIcon(EXEICON);
    }
    QPixmap p = tmp.pixmap(QSize(iconsize, iconsize));
    ui->icon->setPixmap(p);
}

DoubleListItem::DoubleListItem(const QIcon& icon, const QString &text, const QString &subtext, const QString &cmd, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DoubleListItem)
{
    ui->setupUi(this);
    ui->subtext->setStyleSheet("QLabel{color: rgb(100, 100, 100)}");
    ui->cmd->setStyleSheet("QLabel{color: rgb(100, 100, 100)}");
    QFontMetrics fm(QFont(ui->text->font()));
    ui->text->setText(fm.elidedText(text, Qt::ElideRight, ui->text->width()));
    QFontMetrics subfm(QFont(ui->subtext->font()));
    ui->subtext->setText(subfm.elidedText(subtext, Qt::ElideRight, ui->subtext->width()));
    ui->cmd->setText(cmd);
    QIcon tmp = icon;
    QStringList sl;
    if (tmp.isNull())
    {
        tmp = QIcon(EXEICON);
    }
    QPixmap p = tmp.pixmap(QSize(iconsize, iconsize));
    ui->icon->setPixmap(p);
}


DoubleListItem::~DoubleListItem()
{
    delete ui;
}
