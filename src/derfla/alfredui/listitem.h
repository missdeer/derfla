#ifndef LISTITEM_H
#define LISTITEM_H

#include <QWidget>
#include <string>

using std::string;

namespace Ui {
class ListItem;
}

class ListItem : public QWidget
{
    Q_OBJECT

public:
    explicit ListItem(const QString &icon, const QString& text, const QString& cmd, QWidget *parent = nullptr);
    explicit ListItem(const QIcon& icon, const QString& text, const QString& cmd, QWidget *parent = nullptr);

    ~ListItem();

private:
    Ui::ListItem *ui;
};

#endif // LISTITEM_H
