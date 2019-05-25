#include "stdafx.h"
#include "doublelistitem.h"
#include "ui_doublelistitem.h"
#include <QStringList>
#include <QString>
#include <QIcon>

static const int iconSize = 42;
static const QString exeIcon = ":/rc/actions/terminal.png";

DoubleListItem::DoubleListItem(const QString& icon, const QString& title, const QString& description, const QString& cmd, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DoubleListItem)
{
    ui->setupUi(this);
    ui->subtext->setStyleSheet("QLabel{color: rgb(100, 100, 100)}");
    ui->cmd->setStyleSheet("QLabel{color: rgb(100, 100, 100)}");
    QFont textFont(ui->text->font());
    textFont.setFamily(globalDefaultFontFamily);
    QFontMetrics fm(textFont);
    ui->text->setFont(textFont);
    ui->text->setText(fm.elidedText(title, Qt::ElideRight, ui->text->width()));
    QFont subtextFont(ui->subtext->font());
    subtextFont.setFamily(globalDefaultFontFamily);
    QFontMetrics subfm(subtextFont);
    ui->subtext->setFont(subtextFont);
    ui->subtext->setText(subfm.elidedText(description, Qt::ElideRight, ui->subtext->width()));
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

DoubleListItem::DoubleListItem(const QIcon& icon, const QString &text, const QString &description, const QString &cmd, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DoubleListItem)
{
    ui->setupUi(this);
    ui->subtext->setStyleSheet("QLabel{color: rgb(100, 100, 100)}");
    ui->cmd->setStyleSheet("QLabel{color: rgb(100, 100, 100)}");
    QFont textFont(ui->text->font());
    textFont.setFamily(globalDefaultFontFamily);
    QFontMetrics fm(textFont);
    ui->text->setFont(textFont);
    ui->text->setText(fm.elidedText(text, Qt::ElideRight, ui->text->width()));
    QFont subtextFont(ui->subtext->font());
    subtextFont.setFamily(globalDefaultFontFamily);
    QFontMetrics subfm(subtextFont);
    ui->subtext->setFont(subtextFont);
    ui->subtext->setText(subfm.elidedText(description, Qt::ElideRight, ui->subtext->width()));
    ui->cmd->setText(cmd);
    QIcon tmp = icon;
    QStringList sl;
    if (tmp.isNull())
    {
        tmp = QIcon(exeIcon);
    }
    QPixmap p = tmp.pixmap(QSize(iconSize, iconSize));
    ui->icon->setPixmap(p);
}


DoubleListItem::~DoubleListItem()
{
    delete ui;
}
