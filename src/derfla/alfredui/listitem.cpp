#include "listitem.h"
#include "ui_listitem.h"
#include "config.h"
#include <string>
#include <QString>
#include <QIcon>
#include <QFont>
#include <QFontMetrics>
#include <QPixmap>

ListItem::ListItem(const QString& icon, const QString &text, const QString &cmd, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListItem)
{
    ui->setupUi(this);
    ui->cmd->setStyleSheet("QLabel{color: rgb(100, 100, 100)}");
    QFontMetrics fm(QFont(ui->text->font()));
    ui->text->setText(fm.elidedText(text, Qt::ElideRight, ui->text->width()));
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

ListItem::ListItem(const QIcon &icon, const QString &text, const QString &cmd, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListItem)
{
    ui->setupUi(this);
    ui->cmd->setStyleSheet("QLabel{color: rgb(100, 100, 100)}");
    QFontMetrics fm(QFont(ui->text->font()));
    ui->text->setText(fm.elidedText(text, Qt::ElideRight, ui->text->width()));
    ui->cmd->setText(cmd);
    QIcon tmp = icon;
    if (tmp.isNull())
    {
        tmp = QIcon(EXEICON);
    }
    QPixmap p = tmp.pixmap(QSize(iconsize, iconsize));
    ui->icon->setPixmap(p);
}

ListItem::~ListItem()
{
    delete ui;
}
