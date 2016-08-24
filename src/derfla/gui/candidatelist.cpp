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

    setMinimumSize(10, 10);
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

    dal_.clear();
    ui->list->clear();
    DBRW::instance()->getLFSActions(dal_, text, 25);
    populateList();
}

void CandidateList::update(AlfredWorkflowPtr aw, const QString &text)
{
    if (text.isEmpty())
        hide();

    dal_.clear();
    ui->list->clear();
    dal_ = aw->getActions(text); 
    if (dal_.isEmpty())
        DBRW::instance()->getLFSActions(dal_, text, 25);
    populateList();
}

void CandidateList::populateList()
{
    if (dal_.empty())
        hide();
    else
    {
        for (DerflaActionPtr da : dal_)
        {
            QListWidgetItem* item = new QListWidgetItem(ui->list);
            item->setData(Qt::DisplayRole, da->title());
            item->setData(Qt::UserRole + 1, da->description());
            item->setData(Qt::DecorationRole, da->icon());
            ui->list->addItem(item);
        }
        itemCount_ = dal_.length();
        refreshList();
    }
}

void CandidateList::refreshList()
{
    if (itemCount_ > 0)
    {
        ui->list->setCurrentRow(0);
        QSize s = size();
        resize(s.width(), qMin(10, itemCount_) * CandidateListItemHeight);
        //qDebug() << itemCount_ << s << size() << s.width() << qMin(10, itemCount_) * CandidateListItemHeight;
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
        //qDebug() << "CandidateList::keyPressEvent: event->key() == Qt::Key_Down";
    }
    else
    {
        //qDebug() << "CandidateList::keyPressEvent: ignore" << event;
        event->ignore();
    }
}

void CandidateList::showEvent(QShowEvent* /*event*/)
{
    refreshList();
}

void CandidateList::onEnter()
{
    check_expiration;
    int index = ui->list->currentRow();
    //qDebug() << "CandidateList::onEnter:" << index;
    if (index < 0 || index >= itemCount_)
    {
        close();
        return;
    }
    DerflaActionPtr da = dal_.at(index);
    da->run();
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
