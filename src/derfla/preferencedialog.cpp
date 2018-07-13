#include "stdafx.h"
#include "preferencedialog.h"
#include "ui_preferencedialog.h"

PreferenceDialog::PreferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);
    connect(ui->listWidget, &QListWidget::currentRowChanged, ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    // general settings page
    QVBoxLayout* gl = new QVBoxLayout(ui->generalPage);
    QCheckBox* cbStayOnTop = new QCheckBox(tr("Stay On Top"), ui->generalPage);
    gl->addWidget(cbStayOnTop);

    QHBoxLayout* dl = new QHBoxLayout(ui->generalPage);
    QLabel* label = new QLabel(tr("Delay Interval:"), ui->generalPage);
    dl->addWidget(label);
    QSlider* slider = new QSlider(Qt::Horizontal, ui->generalPage);
    slider->setRange(0, 1000);
    dl->addWidget(slider);
    QSpinBox* spinBox = new QSpinBox(ui->generalPage);
    spinBox->setRange(0, 1000);
    dl->addWidget(spinBox);
    gl->addLayout(dl);

    QHBoxLayout* hl = new QHBoxLayout(ui->generalPage);
    QLabel* l = new QLabel(tr("Global Hotkey:"), ui->generalPage);
    hl->addWidget(l);
    QKeySequenceEdit* hk = new QKeySequenceEdit(ui->generalPage);
    hl->addWidget(hk);
    gl->addLayout(hl);

    QCheckBox* cbAutoUpdate = new QCheckBox(tr("Auto Update"), ui->generalPage);
    gl->addWidget(cbAutoUpdate);

    gl->addStretch(1);

    ui->generalPage->setLayout(gl);
    // extension settings page

    // skin settings page

    ui->stackedWidget->setCurrentIndex(0);
}

PreferenceDialog::~PreferenceDialog()
{
    delete ui;
}
