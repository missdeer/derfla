#ifndef QRCODEDIALOG_H
#define QRCODEDIALOG_H

#include <QDialog>

namespace Ui {
class QrcodeDialog;
}

class QrcodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QrcodeDialog(bool alipay, QWidget *parent = nullptr);
    ~QrcodeDialog();

private:
    Ui::QrcodeDialog *ui;
};

#endif // QRCODEDIALOG_H
