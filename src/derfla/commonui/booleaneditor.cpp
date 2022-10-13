#include "stdafx.h"

#include "booleaneditor.h"

BooleanWidget::BooleanWidget(QWidget *parent) : QWidget(parent), checkBox_(new QCheckBox(this))
{
    auto *layout = new QHBoxLayout(this);
    layout->addWidget(checkBox_, 0, Qt::AlignCenter);
}
bool BooleanWidget::isChecked()
{
    return checkBox_->isChecked();
}
void BooleanWidget::setChecked(bool value)
{
    checkBox_->setChecked(value);
}

BooleanEditor::BooleanEditor(QObject *parent) : QItemDelegate(parent) {}

BooleanEditor::~BooleanEditor() {}

void BooleanEditor::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::DisplayRole).toInt();

    auto *checkbox = qobject_cast<BooleanWidget *>(editor);

    checkbox->setChecked((value == 1));
}

void BooleanEditor::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    auto *checkBox = qobject_cast<BooleanWidget *>(editor);
    auto  value    = checkBox->isChecked() ? Qt::Checked : Qt::Unchecked;

    model->setData(index, value, Qt::DisplayRole);
}

QWidget *BooleanEditor::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    auto *editor = new BooleanWidget(parent);
    connect(editor, SIGNAL(toggled(bool)), this, SLOT(changed(bool)));

    return editor;
}

void BooleanEditor::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    drawCheck(painter, option, option.rect, index.data().toBool() ? Qt::Checked : Qt::Unchecked);
    drawFocus(painter, option, option.rect);
}

void BooleanEditor::changed(bool val)
{
    Q_UNUSED(val);
    auto *checkbox = qobject_cast<BooleanWidget *>(sender());
    emit  commitData(checkbox);
    emit  closeEditor(checkbox);
}
