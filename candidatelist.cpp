#include "candidatelist.h"
#include "ui_candidatelist.h"

CandidateList::CandidateList(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CandidateList)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_ShowWithoutActivating);
}

CandidateList::~CandidateList()
{
    delete ui;
}

void CandidateList::update(const QString &text)
{

}

void CandidateList::on_listWidget_pressed(const QModelIndex &index)
{
    close();
}
