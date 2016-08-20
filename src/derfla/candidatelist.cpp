#include "stdafx.h"
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
    for (int i = 0; i < 12; i++)
    {
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(Qt::DisplayRole, QString( "Title %1").arg(i));
        item->setData(Qt::UserRole + 1, QString("Description %1").arg(i));
        item->setData(Qt::DecorationRole, QIcon(":/derfla.ico"));
        ui->list->addItem(item);
    }

    connect(ui->list, &CandidateListWidget::keyPressedEvent, this, &CandidateList::keyPressedEvent);
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
    check_expiration;
    if (event->key() == Qt::Key_Down)
    {
        if (count() > 1 && getActiveWindowFlag())
        {
            setActiveWindowFlag(false);
            ui->list->setCurrentRow(1);
        }
        qDebug() << "event->key() == Qt::Key_Down" << __FUNCTION__;
    }
    else
    {
        qDebug() << "ignore" << __FUNCTION__ << event;
        event->ignore();
    }
}

void CandidateList::showEvent(QShowEvent *event)
{
    if (ui->list->count() > 0)
        ui->list->setCurrentRow(0);
    QSize s = size();
    resize(s.width(), qMin(10, ui->list->count()) * 60);
}

void CandidateList::on_listWidget_pressed(const QModelIndex &index)
{
    qDebug() << __FUNCTION__;
    onEnter();
}

void CandidateList::onEnter()
{
    check_expiration;
    int index = ui->list->currentRow();
    qDebug() << __FUNCTION__ << index;
    close();
    emit done();
}

bool CandidateList::getActiveWindowFlag() const
{
    return activeWindowFlag;
}

void CandidateList::setActiveWindowFlag(bool value)
{
    activeWindowFlag = value;
}
