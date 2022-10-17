#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include <QDialog>

#include "extension.h"

namespace Ui
{
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
    explicit PreferenceDialog(QWidget *parent = nullptr);
    ~PreferenceDialog() override;

private slots:
    void on_buttonBox_accepted();
    void onKeySequenceChanged(const QKeySequence &keySequence);
    void onCurrentSkinChanged(const QString &name);
    void onAlfredStyleUIStateChanged(int state);
    void on_btnBrowseBash_clicked();

    void on_btnDetectBash_clicked();

    void on_btnBrowseSh_clicked();

    void on_btnDetectSh_clicked();

    void on_btnBrowseNode_clicked();

    void on_btnDetectNode_clicked();

    void on_btnBrowseTcl_clicked();

    void on_btnDetectTcl_clicked();

    void on_btnBrowsePHP_clicked();

    void on_btnDetectPHP_clicked();

    void on_btnBrowseRuby_clicked();

    void on_btnDetectRuby_clicked();

    void on_btnBrowsePython_clicked();

    void on_btnDetectPython_clicked();

    void on_btnBrowsePerl_clicked();

    void on_btnDetectPerl_clicked();

    void on_btnBrowseZsh_clicked();

    void on_btnDetectZsh_clicked();

    void on_btnBrowseLua_clicked();

    void on_btnDetectLua_clicked();

    void on_btnBrowseAppleScript_clicked();

    void on_btnDetectAppleScript_clicked();

    void on_btnBrowseCScript_clicked();

    void on_btnDetectCScript_clicked();

    void on_btnBrowseWScript_clicked();

    void on_btnDetectWScript_clicked();

private:
    Ui::PreferenceDialog *ui;
    QCheckBox            *cbStayOnTop_;
    QCheckBox            *cbStartWithSystem_;
    QSlider              *sliderInterval_;
    QSpinBox             *sbInterval_;
    QKeySequenceEdit     *edtHotkey_;
    QTableView           *listExtensions_;
    QCheckBox            *cbAlfredStyleUI_;
    QComboBox            *cbSkins_;
    QWidget              *previewSkin_;
    void                  loadThemes();
    void                  loadSkins();
};

#endif // PREFERENCEDIALOG_H
