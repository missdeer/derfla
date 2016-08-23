#include "stdafx.h"
#include "dbrw.h"
#include "candidatelistdelegate.h"
#include "candidatelist.h"
#include "ui_candidatelist.h"

CandidateList::CandidateList(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CandidateList),
    itemCount_(0)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_ShowWithoutActivating);
#if defined(Q_OS_MAC)
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
#elif defined(Q_OS_WIN)
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
#endif

    ui->list->setItemDelegate(new CandidateListDelegate(ui->list));
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

    DerflaActionList dal;
    DBRW::instance()->getLFSActions(dal, text, 25);
    itemCount_ = dal.length();
    ui->list->clear();
    for (DerflaActionPtr da: dal)
    {
        QListWidgetItem* item = new QListWidgetItem(ui->list);
        item->setData(Qt::DisplayRole, da->title());
        item->setData(Qt::UserRole + 1, da->description());
        item->setData(Qt::DecorationRole, da->icon());
        ui->list->addItem(item);
    }
}

int CandidateList::count() const
{
    return itemCount_;
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
        qDebug() << "CandidateList::keyPressEvent: event->key() == Qt::Key_Down";
    }
    else
    {
        qDebug() << "CandidateList::keyPressEvent: ignore" << event;
        event->ignore();
    }
}

void CandidateList::showEvent(QShowEvent *event)
{
    if (itemCount_ > 0)
        ui->list->setCurrentRow(0);
    QSize s = size();
    resize(s.width(), qMin(10, itemCount_) * CandidateListItemHeight);
}

void CandidateList::on_listWidget_pressed(const QModelIndex &index)
{
    qDebug() << "CandidateList::on_listWidget_pressed";
    onEnter();
}

void CandidateList::onEnter()
{
    check_expiration;
    int index = ui->list->currentRow();
    qDebug() << "CandidateList::onEnter:" << index;
    close();
    emit done();
}

bool CandidateList::getActiveWindowFlag() const
{
    return activeWindowFlag_;
}

void CandidateList::setActiveWindowFlag(bool value)
{
    activeWindowFlag_ = value;
}
