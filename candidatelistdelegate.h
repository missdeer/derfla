#ifndef CANDIDATELISTDELEGATE_H
#define CANDIDATELISTDELEGATE_H

#include <QAbstractItemDelegate>
#include <QObject>
#include <QPainter>
#include <QStyleOptionViewItem>

class CandidateListDelegate : public QAbstractItemDelegate
{
public:
    CandidateListDelegate(QObject *parent = 0);

    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    virtual ~CandidateListDelegate();
};

#endif // CANDIDATELISTDELEGATE_H