#include "stdafx.h"

#include <string>

#include <QFont>
#include <QFontMetrics>
#include <QIcon>
#include <QPixmap>
#include <QString>

#include "listitem.h"
#include "ui_listitem.h"

static const int     iconSize = 42;
static const QString exeIcon  = ":/rc/actions/terminal.png";

ListItem::ListItem(const QString &icon, const QString &text, const QString &cmd, QWidget *parent) : QWidget(parent), ui(new Ui::ListItem)
{
    ui->setupUi(this);
    ui->cmd->setStyleSheet("QLabel{color: rgb(100, 100, 100)}");
    QFont textFont(ui->text->font());
    textFont.setFamily(globalDefaultFontFamily);
    QFontMetrics fm(textFont);
    ui->text->setFont(textFont);
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
        tmp = QIcon(exeIcon);
    }
    QPixmap p = tmp.pixmap(QSize(iconSize, iconSize));
    ui->icon->setPixmap(p);
}

ListItem::ListItem(const QIcon &icon, const QString &text, const QString &cmd, QWidget *parent) : QWidget(parent), ui(new Ui::ListItem)
{
    ui->setupUi(this);
    ui->cmd->setStyleSheet("QLabel{color: rgb(100, 100, 100)}");
    QFont textFont(ui->text->font());
    textFont.setFamily(globalDefaultFontFamily);
    QFontMetrics fm(textFont);
    ui->text->setFont(textFont);
    ui->text->setText(fm.elidedText(text, Qt::ElideRight, ui->text->width()));
    ui->cmd->setText(cmd);
    QIcon tmp = icon;
    if (tmp.isNull())
    {
        tmp = QIcon(exeIcon);
    }
    QPixmap p = tmp.pixmap(QSize(iconSize, iconSize));
    ui->icon->setPixmap(p);
}

ListItem::~ListItem()
{
    delete ui;
}
