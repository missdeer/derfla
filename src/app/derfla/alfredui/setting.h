#ifndef SETTING_H
#define SETTING_H

#include <sstream>
#include <string>

#include <QProcess>
#include <QString>
#include <QStringList>
#include <QWidget>

#include "config.h"
#include "hotkey.h"

namespace Ui
{
    class Setting;
}

class Setting : public QWidget
{
    Q_OBJECT
public:
    explicit Setting(QWidget *parent = 0);
    ~Setting();
    void openScript();
    void openIcon();
    void pressHotkey();
    void addItem();
    void deleteItem();
    void modifyItem();
    void saveItems();
    void changeItem();

private:
    Ui::Setting *ui;
};

#endif // SETTING_H
