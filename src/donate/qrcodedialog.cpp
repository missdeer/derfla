#include "qrcodedialog.h"
#include "ui_qrcodedialog.h"

QrcodeDialog::QrcodeDialog(bool alipay, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QrcodeDialog)
{
    ui->setupUi(this);
    if (alipay)
    {
        setWindowTitle(tr("Scan by Alipay"));
        ui->label->setPixmap(QPixmap(":rc/qrcode/alipay.jpg"));
    }
    else
    {
        setWindowTitle(tr("Scan by WeChat"));
        ui->label->setPixmap(QPixmap(":rc/qrcode/wepay.jpg"));
    }
    setFixedSize(size());
}

QrcodeDialog::~QrcodeDialog()
{
    delete ui;
}
