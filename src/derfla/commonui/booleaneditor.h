#ifndef BOOLEANEDITOR_H
#define BOOLEANEDITOR_H

#include <QWidget>
#include <QItemDelegate>
#include <QCheckBox>
#include <QHBoxLayout>

class BooleanWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BooleanWidget(QWidget * parent = nullptr)
        : QWidget(parent)
    {
        checkBox = new QCheckBox(this);
        QHBoxLayout * layout = new QHBoxLayout(this);
        layout->addWidget(checkBox,0, Qt::AlignCenter);
    }

    bool isChecked(){return checkBox->isChecked();}
    void setChecked(bool value){checkBox->setChecked(value);}

private:
    QCheckBox * checkBox;
};

class BooleanEditor : public QItemDelegate
{
    Q_OBJECT
private:
    BooleanWidget *checkbox;

public:
    explicit BooleanEditor(QObject *parent = nullptr);
    ~BooleanEditor();
    void setEditorData( QWidget *editor,const QModelIndex &index )const;
    void setModelData( QWidget *editor,QAbstractItemModel *model,const QModelIndex &index )const;
    QWidget *createEditor( QWidget *parent,const QStyleOptionViewItem &/* option */,const QModelIndex &/* index */ )const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

public slots:
    void changed( bool );

};

#endif // BOOLEANEDITOR_H
