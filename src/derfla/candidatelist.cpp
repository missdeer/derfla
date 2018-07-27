#include "stdafx.h"
#include "extensionmanager.h"
#include "candidatelistdelegate.h"
#include "candidatelist.h"
#include "ui_candidatelist.h"

CandidateList::CandidateList(ExtensionManager* extensionManager, QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::CandidateList)
    , cleared_(true)
    , itemCount_(0)
    , extensionManager_(extensionManager)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setFocusPolicy(Qt::NoFocus);
#if defined(Q_OS_WIN)
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
#else
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip | Qt::WindowStaysOnTopHint);
#endif

    setMinimumSize(10, 10);
    ui->list->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->list->setItemDelegate(new CandidateListDelegate(ui->list));
    connect(ui->list, &CandidateListWidget::keyPressedEvent, this, &CandidateList::keyPressedEvent);
    connect(extensionManager_, &ExtensionManager::actionUpdated, this, &CandidateList::actionUpdated);
    connect(extensionManager_, &ExtensionManager::emptyAction, this, &CandidateList::emptyAction);
}

CandidateList::~CandidateList()
{
    delete ui;
}

void CandidateList::update(const QString &text)
{
    if (text.isEmpty() && isVisible())
        hide();
    else
        cleared_ = false;

    dal_.clear();
    extensionManager_->query(text);
}

void CandidateList::populateList()
{
    if (dal_.empty())
    {
        if (isVisible())
            hide();
    }
    else
    {
#if !defined(Q_OS_MAC) && !defined(Q_OS_WIN)
        std::sort(dal_.begin(), dal_.end(), [](DerflaActionPtr da, DerflaActionPtr ) {
            return !da->icon().isNull();                
            });

remove_duplicated:
        auto it = std::find_if(dal_.begin(), dal_.end(), [&](DerflaActionPtr da) {
            if (da->actionType() == DAT_CONSOLE)
            {
                auto findIt = std::find_if(dal_.begin(), dal_.end(), [da](DerflaActionPtr d) {
                    return (d->actionType() == DAT_GUI && da->target() == d->target() && da->arguments() == d->arguments() );
                });
                return dal_.end() != findIt;
            }
            return false;
            });
        if (dal_.end() != it)
        {
            dal_.erase(it);
            goto remove_duplicated;
        }
#endif
        auto oldCount = ui->list->count();
        for (DerflaActionPtr da : dal_)
        {
            QListWidgetItem* item = new QListWidgetItem(ui->list);
            item->setData(Qt::DisplayRole, da->title());
            item->setData(Qt::UserRole + 1, da->description());
            item->setData(Qt::DecorationRole, da->icon());
            ui->list->addItem(item);
        }
        for (int i = 0; i < oldCount; i++)
            delete ui->list->takeItem(0);
        itemCount_ = dal_.length();
        refreshList();
    }
}

void CandidateList::refreshList()
{
    if (itemCount_ > 0)
    {
        ui->list->setCurrentRow(0);
        if (isHidden())
        {
            show();
        }
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

#if defined(Q_OS_MAC)
    switch(event->key())
    {
    case Qt::Key_Up:
        if (ui->list->currentRow() == 0)
        {
            event->ignore();
        }
        else
        {
            qApp->sendEvent(ui->list, event);
        }
        break;
    case Qt::Key_Down:
        if (ui->list->currentRow() == ui->list->count() -1)
        {
            event->ignore();
        }
        else
        {
            qApp->sendEvent(ui->list, event);
        }
        break;
    default:
        event->ignore();
        break;
    }
#else
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
#endif
}

void CandidateList::showEvent(QShowEvent* /*event*/)
{
    refreshList();
}

void CandidateList::actionUpdated(DerflaActionList &dal)
{
    for(auto da : dal)
    {
        dal_.append(da);
    }
    populateList();
}

void CandidateList::emptyAction()
{
    ui->list->clear();
    dal_.clear();
    populateList();
}

void CandidateList::onEnter()
{
    int index = ui->list->currentRow();
    //qDebug() << "CandidateList::onEnter:" << index;
    if (index < 0 || index >= itemCount_)
    {
        close();
        return;
    }
    DerflaActionPtr da = dal_.at(index);
    if (!da->disabled())
    {
        actionExecutor_(da);
        emit done();
    }
}

void CandidateList::setInputBoxSize(const QSize &s)
{
    resize(qMax(s.width(), 700), size().height());
}

bool CandidateList::getActiveWindowFlag() const
{
    return activeWindowFlag_;
}

void CandidateList::setActiveWindowFlag(bool value)
{
    activeWindowFlag_ = value;
}

void CandidateList::clear()
{
    cleared_ = true;
}
