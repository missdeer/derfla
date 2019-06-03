#include "stdafx.h"
#include "derflaapp.h"
#include "extensionmodel.h"
#include "booleaneditor.h"
#include "preferencedialog.h"
#include "ui_preferencedialog.h"

PreferenceDialog::PreferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);
    connect(ui->listWidget, &QListWidget::currentRowChanged, ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    QSettings &settings = derflaApp->settings();
    // general settings page
    auto* gl = new QVBoxLayout(ui->generalPage);
    cbStayOnTop_ = new QCheckBox(tr("Stay On Top"), ui->generalPage);
    cbStayOnTop_->setChecked(settings.value("stayOnTop", false).toBool());
    gl->addWidget(cbStayOnTop_);

    auto* dl = new QHBoxLayout(ui->generalPage);
    auto* label = new QLabel(tr("Delay Interval:"), ui->generalPage);
    dl->addWidget(label);
    sliderInterval_ = new QSlider(Qt::Horizontal, ui->generalPage);
    sliderInterval_->setRange(0, 1000);
    sliderInterval_->setValue(settings.value("interval", 0).toInt());
    dl->addWidget(sliderInterval_);
    sbInterval_ = new QSpinBox(ui->generalPage);
    sbInterval_->setRange(0, 1000);
    sbInterval_->setValue(settings.value("interval", 0).toInt());
    dl->addWidget(sbInterval_);
    connect(sliderInterval_, &QSlider::valueChanged, sbInterval_, &QSpinBox::setValue);
    gl->addLayout(dl);

    auto* hl = new QHBoxLayout(ui->generalPage);
    auto* l = new QLabel(tr("Global Hotkey:"), ui->generalPage);
    hl->addWidget(l);
    edtHotkey_ = new QKeySequenceEdit(ui->generalPage);
    edtHotkey_->setKeySequence(QKeySequence(settings.value("hotkey", "Alt+Space").toString()));
    connect(edtHotkey_, &QKeySequenceEdit::keySequenceChanged, this, &PreferenceDialog::onKeySequenceChanged);

    hl->addWidget(edtHotkey_);
    gl->addLayout(hl);

    cbAutoUpdate_ = new QCheckBox(tr("Auto Update"), ui->generalPage);
    cbAutoUpdate_->setChecked(settings.value("autoupdate", true).toBool());
    gl->addWidget(cbAutoUpdate_);

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    cbStartWithSystem_ = new QCheckBox(tr("Start with system"), ui->generalPage);
    cbStartWithSystem_->setChecked(settings.value("autostart", false).toBool());
    gl->addWidget(cbStartWithSystem_);
#endif

    gl->addStretch(1);

    ui->generalPage->setLayout(gl);
    // extension settings page
    auto* gl2 = new QVBoxLayout(ui->extensionPage);

    listExtensions_ = new QTableView(ui->extensionPage);
    auto extensionModel = new ExtensionModel(listExtensions_);
    listExtensions_->setItemDelegateForColumn(4, new BooleanEditor(listExtensions_));
    listExtensions_->setModel(extensionModel);
    listExtensions_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    gl2->addWidget(listExtensions_);

    gl2->setContentsMargins(0,0,0,0);
    ui->extensionPage->setLayout(gl2);
    // skin settings page
    auto* gl3 = new QVBoxLayout(ui->skinPage);

    cbAlfredStyleUI_ = new QCheckBox(tr("Use Alfred Style User Interface"), ui->skinPage);
    cbAlfredStyleUI_->setChecked(settings.value("alfredStyleUI").toBool());
    gl3->addWidget(cbAlfredStyleUI_);
    
    cbSkins_ = new QComboBox(ui->skinPage);
    if (cbAlfredStyleUI_->isChecked())
    {
#if defined(Q_OS_MAC)
        QDir dir(QCoreApplication::applicationDirPath() % "/../Resources/themes/");
        QString skinPath = settings.value("theme", isDarkMode() ? ":/themes/dark.derflatheme" : ":/themes/classic.derflatheme").toString();
#else
        QDir dir(QCoreApplication::applicationDirPath() % "/themes");
        QString skinPath = settings.value("theme", ":/themes/classic.derflatheme").toString();
#endif
        auto eil = dir.entryInfoList(QStringList() << "*.derflatheme", QDir::Files);
        for (const auto& fi : eil)
        {
            cbSkins_->addItem(fi.fileName());
        }
        int index = cbSkins_->findText(skinPath);
        if (index >= 0)
            cbSkins_->setCurrentIndex(index);
        else
            cbSkins_->setCurrentText(QFileInfo(skinPath).fileName());
    }
    else 
    {
        cbSkins_->addItem("derfla.derflaskin");
        
#if defined(Q_OS_MAC)
        QDir dir(QCoreApplication::applicationDirPath() % "/../Resources/skins/");
#else
        QDir dir(QCoreApplication::applicationDirPath() % "/skins");
#endif
        auto eil = dir.entryInfoList(QStringList() << "*.derflaskin", QDir::Files);
        for (const auto& fi : eil)
        {
            cbSkins_->addItem(fi.fileName());
        }
        QString skinPath = settings.value("skin", "derfla.derflaskin").toString();
        int index = cbSkins_->findText(skinPath);
        if (index >= 0)
            cbSkins_->setCurrentIndex(index);
        else
            cbSkins_->setCurrentText(QFileInfo(skinPath).fileName());
    }
    
    connect(cbSkins_, &QComboBox::currentTextChanged, this, &PreferenceDialog::onCurrentSkinChanged);
    gl3->addWidget(cbSkins_);

    previewSkin_ = new QWidget(ui->skinPage);
    gl3->addWidget(previewSkin_);

    gl3->addStretch(1);
    ui->skinPage->setLayout(gl3);

    ui->stackedWidget->setCurrentIndex(0);
}

PreferenceDialog::~PreferenceDialog()
{
    delete ui;
}

void PreferenceDialog::on_buttonBox_accepted()
{
    QSettings &settings = derflaApp->settings();

    settings.setValue("stayOnTop", cbStayOnTop_->isChecked());
    settings.setValue("interval", sliderInterval_->value());
    settings.setValue("hotkey", edtHotkey_->keySequence().toString());
    settings.setValue("skin", cbSkins_->currentIndex() == 0 ?
                          cbSkins_->currentText() :
                                                            (QCoreApplication::applicationDirPath() %
                       #if defined(Q_OS_MAC)
                           "/../Resources/skins/"
                       #else
                           "/skins/"
                       #endif
                           % cbSkins_->currentText()));
    settings.setValue("alfredStyleUI", cbAlfredStyleUI_->isChecked());
    settings.setValue("autoupdate", cbAutoUpdate_->isChecked());
#if defined (Q_OS_WIN) || defined(Q_OS_MAC)
    settings.setValue("autostart", cbStartWithSystem_->isChecked());
#endif
    settings.sync();
    QDialog::accept();
}

void PreferenceDialog::onKeySequenceChanged(const QKeySequence &keySequence)
{
    QSettings &settings = derflaApp->settings();
    if (keySequence == QKeySequence(settings.value("hotkey", "Alt+Space").toString()))
        return;
}

void PreferenceDialog::onCurrentSkinChanged(const QString &name)
{

}
