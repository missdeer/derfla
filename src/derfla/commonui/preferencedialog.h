#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include <QDialog>
#include "extension.h"

namespace Ui {
class PreferenceDialog;
}

QT_FORWARD_DECLARE_CLASS(QCheckBox)
QT_FORWARD_DECLARE_CLASS(QSlider)
QT_FORWARD_DECLARE_CLASS(QSpinBox)
QT_FORWARD_DECLARE_CLASS(QKeySequenceEdit)
QT_FORWARD_DECLARE_CLASS(QTableView)
QT_FORWARD_DECLARE_CLASS(QComboBox)

class PreferenceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferenceDialog(const QList<ExtensionPtr>& extensions, QWidget *parent = nullptr);
    ~PreferenceDialog();

private slots:
    void on_buttonBox_accepted();
    void onKeySequenceChanged(const QKeySequence &keySequence);
    void onCurrentSkinChanged(const QString& name);
private:
    Ui::PreferenceDialog *ui;
    QCheckBox* cbStayOnTop_;
    QCheckBox* cbStartWithSystem_;
    QSlider* sliderInterval_;
    QSpinBox* sbInterval_;
    QKeySequenceEdit* edtHotkey_;
    QCheckBox* cbAutoUpdate_;
    QTableView* listExtensions_;
    QCheckBox* cbAlfredStyleUI_;
    QComboBox* cbSkins_;
    QWidget* previewSkin_;
};

#endif // PREFERENCEDIALOG_H
