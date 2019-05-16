#ifndef DOUBLELISTITEM_H
#define DOUBLELISTITEM_H

#include <QWidget>
#include <string>

using std::string;

namespace Ui {
class DoubleListItem;
}

class DoubleListItem : public QWidget
{
    Q_OBJECT

public:
    explicit DoubleListItem(const QString& icon, const QString& text, const QString& subtext, const QString& cmd, QWidget *parent = nullptr);
    explicit DoubleListItem(const QIcon &icon, const QString& text, const QString& subtext, const QString& cmd, QWidget *parent = nullptr);
    ~DoubleListItem();

private:
    Ui::DoubleListItem *ui;
};

#endif // DOUBLELISTITEM_H
