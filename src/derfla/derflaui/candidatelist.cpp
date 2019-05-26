#include "stdafx.h"
#include "derflaapp.h"
#include "extensionmanager.h"
#include "candidatelistdelegate.h"
#include "candidatelist.h"
#include "ui_candidatelist.h"

CandidateList::CandidateList(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::CandidateList)
    , cleared_(true)
    , actionIconMap_{
               {"script",          QIcon(":/rc/actions/script.png")},
               {"shellExecute",    QIcon(":/rc/actions/shell.png")},
               {"terminalCommand", QIcon(":/rc/actions/terminal.png")},
               {"openUrl",         QIcon(":/rc/actions/openurl.png")},
               {"revealFile",      QIcon(":/rc/actions/reveal.png")},
               {"browseInDerfla",  QIcon(":/rc/actions/browse.png")},
               {"copyText",        QIcon(":/rc/actions/copytext.png")},
          }
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
    ui->list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->list->setItemDelegate(new CandidateListDelegate(ui->list));
    connect(ui->list, &CandidateListWidget::keyPressedEvent, this, &CandidateList::keyPressedEvent);
    connect(derflaApp, &DerflaApp::actionUpdated, this, &CandidateList::onActionUpdated);
    connect(derflaApp, &DerflaApp::emptyAction, this, &CandidateList::onEmptyAction);
}

CandidateList::~CandidateList()
{
    delete ui;
}

void CandidateList::clearData()
{
    derflaApp->clearDerflaAction();
}

void CandidateList::update(const QString &text)
{
    if (text.isEmpty() && isVisible())
        hide();
    else
        cleared_ = false;
    
    qDebug() << __FUNCTION__ << text;
    clearData();
    derflaApp->queryByExtension(text);
}

void CandidateList::populateList()
{
    if (derflaApp->isEmptyDerflaAction())
    {
        if (isVisible())
            hide();
    }
    else
    {
        DerflaActionList &dal = derflaApp->derflaActions();
#if !defined(Q_OS_MAC) && !defined(Q_OS_WIN)
        std::sort(dal.begin(), dal.end(), [](DerflaActionPtr da, DerflaActionPtr ) {
            return !da->icon().isNull();                
            });

remove_duplicated:
        auto it = std::find_if(dal.begin(), dal.end(), [&](DerflaActionPtr da) {
            if (da->actionType() == DAT_CONSOLE)
            {
                auto findIt = std::find_if(dal.begin(), dal.end(), [da](DerflaActionPtr d) {
                    return (d->actionType() == DAT_GUI && da->target() == d->target() && da->arguments() == d->arguments() );
                });
                return dal.end() != findIt;
            }
            return false;
            });
        if (dal.end() != it)
        {
            dal.erase(it);
            goto remove_duplicated;
        }
#endif
        auto oldCount = ui->list->count();
        for (DerflaActionPtr da : dal)
        {
            QListWidgetItem* item = new QListWidgetItem(ui->list);
            item->setData(Qt::DisplayRole, da->title());
            item->setData(Qt::UserRole + 1, da->description());
            item->setData(Qt::UserRole + 2, actionIconMap_[da->actionType()]);
            item->setData(Qt::DecorationRole, da->icon());
            ui->list->addItem(item);
        }
        DerflaActionList &dalDonate = derflaApp->donateDerflaActions();
        for (DerflaActionPtr da : dalDonate)
        {
            QListWidgetItem* item = new QListWidgetItem(ui->list);
            item->setData(Qt::DisplayRole, da->title());
            item->setData(Qt::UserRole + 1, da->description());
            item->setData(Qt::UserRole + 2, actionIconMap_[da->actionType()]);
            item->setData(Qt::DecorationRole, da->icon());
            ui->list->addItem(item);
        }
        for (int i = 0; i < oldCount; i++)
            delete ui->list->takeItem(0);
        
        if (!derflaApp->isEmptyDerflaAction())
            refreshList();
    }
}

void CandidateList::refreshList()
{
    if (ui->list->count() > 0)
    {
        ui->list->setCurrentRow(0);
        if (isHidden())
        {
            show();
        }
        QSize s = size();
        resize(s.width(), qMin(10, ui->list->count()) * CandidateListItemHeight);
        //qDebug() << itemCount_ << s << size() << s.width() << qMin(10, itemCount_) * CandidateListItemHeight;
    }
}

int CandidateList::count() const
{
    return ui->list->count();
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
            QCoreApplication::instance()->sendEvent(ui->list, event);
        }
        break;
    case Qt::Key_Down:
        if (ui->list->currentRow() == ui->list->count() -1)
        {
            event->ignore();
        }
        else
        {
            QCoreApplication::instance()->sendEvent(ui->list, event);
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

void CandidateList::onActionUpdated()
{
    //qDebug() << __FUNCTION__ << dal.length() << derflaApp->derflaActionCount();
    populateList();
}

void CandidateList::onEmptyAction()
{
    //qDebug() << __FUNCTION__;
    ui->list->clear();
    clearData();
    populateList();
}

void CandidateList::onEnter()
{
    if (derflaApp->isEmptyDerflaAction())
        return;
    int index = ui->list->currentRow();
    //qDebug() << __FUNCTION__ << index << derflaApp->derflaActionCount() << derflaApp->donateDerflaActions().length();//<< itemCount_;
    if (index < 0 || index >= derflaApp->derflaActionCount() + derflaApp->donateDerflaActions().length())
    {
        close();
        return;
    }
    DerflaActionPtr da = derflaApp->derflaAction(index);
    if (!da->disabled())
    {
        derflaApp->executeAction(da);
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
