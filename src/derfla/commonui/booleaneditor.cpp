#include "stdafx.h"

#include "booleaneditor.h"

BooleanEditor::BooleanEditor(QObject *parent) : QItemDelegate(parent) {}

BooleanEditor::~BooleanEditor() {}

void BooleanEditor::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::DisplayRole).toInt();

    BooleanWidget *checkbox = static_cast<BooleanWidget *>(editor);

    if (value == 1)
    {
        checkbox->setChecked(true);
    }
    else
    {
        checkbox->setChecked(false);
    }
}

void BooleanEditor::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    BooleanWidget *checkBox = qobject_cast<BooleanWidget *>(editor);
    Qt::CheckState value;

    if (checkBox->isChecked())
        value = Qt::Checked;
    else
        value = Qt::Unchecked;

    model->setData(index, value, Qt::DisplayRole);
}

QWidget *BooleanEditor::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    BooleanWidget *editor = new BooleanWidget(parent);
    connect(editor, SIGNAL(toggled(bool)), this, SLOT(changed(bool)));

    return editor;
}

void BooleanEditor::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    drawCheck(painter, option, option.rect, index.data().toBool() ? Qt::Checked : Qt::Unchecked);
    drawFocus(painter, option, option.rect);
}

void BooleanEditor::changed(bool)
{
    BooleanWidget *checkbox = static_cast<BooleanWidget *>(sender());
    emit           commitData(checkbox);
    emit           closeEditor(checkbox);
}
