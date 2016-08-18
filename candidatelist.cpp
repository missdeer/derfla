#include <QtCore>
#include <QKeyEvent>
#include "candidatelistdelegate.h"
#include "candidatelist.h"
#include "ui_candidatelist.h"

CandidateList::CandidateList(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CandidateList)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_ShowWithoutActivating);
#if defined(Q_OS_MAC)
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
#elif defined(Q_OS_WIN)
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
#endif

    ui->list->setItemDelegate(new CandidateListDelegate(ui->list));
    QListWidgetItem *item = new QListWidgetItem();
    item->setData(Qt::DisplayRole, "Title 1");
    item->setData(Qt::UserRole + 1, "Description 1");
    ui->list->addItem(item);
    item = new QListWidgetItem();
    item->setData(Qt::DisplayRole, "Title 2");
    item->setData(Qt::UserRole + 1, "Description 2");
    ui->list->addItem(item);
    item = new QListWidgetItem();
    item->setData(Qt::DisplayRole, "Title 3");
    item->setData(Qt::UserRole + 1, "Description 3");
    ui->list->addItem(item);
}

CandidateList::~CandidateList()
{
    delete ui;
}

void CandidateList::update(const QString &text)
{
    if (text.isEmpty())
        hide();
}

int CandidateList::count() const
{
    return ui->list->count();
}

void CandidateList::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        close();
    }
    else if (event->key() == Qt::Key_Down)
    {
        if (count() > 1 && getActiveWindowFlag())
        {
            setActiveWindowFlag(false);
            ui->list->setCurrentRow(1);
        }
        qDebug() << __PRETTY_FUNCTION__;
    }
    else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        qDebug() << "enter" << __PRETTY_FUNCTION__;
        onEnter();
    }
    else
    {
        event->ignore();
    }
}

void CandidateList::showEvent(QShowEvent *event)
{
    if (ui->list->count() > 0)
        ui->list->setCurrentRow(0);
}

void CandidateList::on_listWidget_pressed(const QModelIndex &index)
{
    qDebug() << __PRETTY_FUNCTION__;
    onEnter();
}

void CandidateList::onEnter()
{
    qDebug() << __PRETTY_FUNCTION__;
    close();
}

bool CandidateList::getActiveWindowFlag() const
{
    return activeWindowFlag;
}

void CandidateList::setActiveWindowFlag(bool value)
{
    activeWindowFlag = value;
}
