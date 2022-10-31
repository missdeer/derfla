#ifndef DOUBLELISTITEM_H
#define DOUBLELISTITEM_H

#include <string>

#include <QWidget>

using std::string;

namespace Ui
{
    class DoubleListItem;
}

class DoubleListItem : public QWidget
{
    Q_OBJECT

public:
    explicit DoubleListItem(const QString &icon, const QString &title, const QString &description, const QString &cmd, QWidget *parent = nullptr);
    explicit DoubleListItem(const QIcon &icon, const QString &title, const QString &description, const QString &cmd, QWidget *parent = nullptr);
    ~DoubleListItem();

private:
    Ui::DoubleListItem *ui;
};

#endif // DOUBLELISTITEM_H
