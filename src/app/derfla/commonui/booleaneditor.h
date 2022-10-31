#ifndef BOOLEANEDITOR_H
#define BOOLEANEDITOR_H

#include <QCheckBox>
#include <QHBoxLayout>
#include <QItemDelegate>
#include <QWidget>

class BooleanWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BooleanWidget(QWidget *parent = nullptr);

    bool isChecked();
    void setChecked(bool value);

    QCheckBox *checkBox();

private:
    QCheckBox *checkBox_;
};

class BooleanEditor : public QItemDelegate
{
    Q_OBJECT
private:
    BooleanWidget *checkbox_ {nullptr};

public:
    explicit BooleanEditor(QObject *parent = nullptr);
    ~BooleanEditor() override;
    void     setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void     setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem & /* option */, const QModelIndex & /* index */) const override;
    void     paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private slots:
    void changed(bool);
};

#endif // BOOLEANEDITOR_H
