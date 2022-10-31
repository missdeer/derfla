#include "stdafx.h"

#include "preferencedialog.h"
#include "booleaneditor.h"
#include "derflaapp.h"
#include "extensionmodel.h"
#include "ui_preferencedialog.h"
#include "util.h"

#if defined(Q_OS_MAC)
#    include "darkmode.h"
#endif

PreferenceDialog::PreferenceDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);
    connect(ui->listWidget, &QListWidget::currentRowChanged, ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    QSettings &settings = derflaApp->settings();
    // general settings page
    auto *gl     = new QVBoxLayout(ui->generalPage);
    cbStayOnTop_ = new QCheckBox(tr("Stay On Top"), ui->generalPage);
    cbStayOnTop_->setChecked(settings.value("stayOnTop", false).toBool());
    gl->addWidget(cbStayOnTop_);

    auto *dl    = new QHBoxLayout(ui->generalPage);
    auto *label = new QLabel(tr("Delay Interval:"), ui->generalPage);
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

    auto *hl = new QHBoxLayout(ui->generalPage);
    auto *l  = new QLabel(tr("Global Hotkey:"), ui->generalPage);
    hl->addWidget(l);
    edtHotkey_ = new QKeySequenceEdit(ui->generalPage);
    edtHotkey_->setKeySequence(QKeySequence(settings.value("hotkey", "Alt+Space").toString()));
    connect(edtHotkey_, &QKeySequenceEdit::keySequenceChanged, this, &PreferenceDialog::onKeySequenceChanged);

    hl->addWidget(edtHotkey_);
    gl->addLayout(hl);

#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    cbStartWithSystem_ = new QCheckBox(tr("Start with system"), ui->generalPage);
    cbStartWithSystem_->setChecked(settings.value("autostart", false).toBool());
    gl->addWidget(cbStartWithSystem_);
#endif

    gl->addStretch(1);

    ui->generalPage->setLayout(gl);
    // extension settings page
    auto *gl2 = new QVBoxLayout(ui->extensionPage);

    listExtensions_      = new QTableView(ui->extensionPage);
    auto *extensionModel = new ExtensionModel(listExtensions_);
    listExtensions_->setItemDelegateForColumn(4, new BooleanEditor(listExtensions_));
    listExtensions_->setModel(extensionModel);
    listExtensions_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    gl2->addWidget(listExtensions_);

    gl2->setContentsMargins(0, 0, 0, 0);
    ui->extensionPage->setLayout(gl2);
    // skin settings page
    auto *gl3 = new QVBoxLayout(ui->skinPage);

    cbAlfredStyleUI_ = new QCheckBox(tr("Use Alfred Style User Interface"), ui->skinPage);
    connect(cbAlfredStyleUI_, &QCheckBox::stateChanged, this, &PreferenceDialog::onAlfredStyleUIStateChanged);

    cbSkins_ = new QComboBox(ui->skinPage);
    connect(cbSkins_, &QComboBox::currentTextChanged, this, &PreferenceDialog::onCurrentSkinChanged);

    cbAlfredStyleUI_->setChecked(settings.value("alfredStyleUI").toBool());
    gl3->addWidget(cbAlfredStyleUI_);

    gl3->addWidget(cbSkins_);

    previewSkin_ = new QWidget(ui->skinPage);
    gl3->addWidget(previewSkin_);

    gl3->addStretch(1);
    ui->skinPage->setLayout(gl3);

    ui->edtBashPath->setText(settings.value("bash").toString());
    ui->edtZshPath->setText(settings.value("zsh").toString());
    ui->edtPythonPath->setText(settings.value("python").toString());
    ui->edtRubyPath->setText(settings.value("ruby").toString());
    ui->edtPHPPath->setText(settings.value("php").toString());
    ui->edtLuaPath->setText(settings.value("lua").toString());
    ui->edtNodePath->setText(settings.value("node").toString());
    ui->edtTclPath->setText(settings.value("tcl").toString());
    ui->edtShPath->setText(settings.value("sh").toString());
    ui->edtPerlPath->setText(settings.value("perl").toString());
#if defined(Q_OS_MAC)
    ui->edtAppleScriptPath->setText(settings.value("applescript(as)").toString());
    ui->edtWScriptPath->setEnabled(false);
    ui->edtCScriptPath->setEnabled(false);
    ui->btnBrowseWScript->setEnabled(false);
    ui->btnBrowseCScript->setEnabled(false);
    ui->btnDetectWScript->setEnabled(false);
    ui->btnDetectCScript->setEnabled(false);
#endif
#if defined(Q_OS_WIN)
    ui->edtAppleScriptPath->setEnabled(false);
    ui->btnBrowseAppleScript->setEnabled(false);
    ui->btnDetectAppleScript->setEnabled(false);
    ui->edtWScriptPath->setText(settings.value("wscript").toString());
    ui->edtCScriptPath->setText(settings.value("cscript").toString());
#endif
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
    settings.setValue("alfredStyleUI", cbAlfredStyleUI_->isChecked());
    if (cbAlfredStyleUI_->isChecked())
    {
        settings.setValue("theme",
                          cbSkins_->currentIndex() == 0 ? cbSkins_->currentText()
                                                        : (QCoreApplication::applicationDirPath() %
#if defined(Q_OS_MAC)
                                                           "/../Resources/themes/"
#else
                                                           "/themes/"
#endif
                                                           % cbSkins_->currentText()));
    }
    else
    {
        settings.setValue("skin",
                          cbSkins_->currentIndex() == 0 ? cbSkins_->currentText()
                                                        : (QCoreApplication::applicationDirPath() %
#if defined(Q_OS_MAC)
                                                           "/../Resources/skins/"
#else
                                                           "/skins/"
#endif
                                                           % cbSkins_->currentText()));
    }
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    settings.setValue("autostart", cbStartWithSystem_->isChecked());
#endif
    settings.setValue("bash", ui->edtBashPath->text());
    settings.setValue("zsh", ui->edtZshPath->text());
    settings.setValue("python", ui->edtPythonPath->text());
    settings.setValue("ruby", ui->edtRubyPath->text());
    settings.setValue("php", ui->edtPHPPath->text());
    settings.setValue("lua", ui->edtLuaPath->text());
    settings.setValue("node", ui->edtNodePath->text());
    settings.setValue("tcl", ui->edtTclPath->text());
    settings.setValue("sh", ui->edtShPath->text());
    settings.setValue("perl", ui->edtPerlPath->text());
#if defined(Q_OS_MAC)
    settings.setValue("applescript(as)", ui->edtAppleScriptPath->text());
    settings.setValue("applescript(js)", ui->edtAppleScriptPath->text());
#endif
#if defined(Q_OS_WIN)
    settings.setValue("wscript", ui->edtWScriptPath->text());
    settings.setValue("cscript", ui->edtCScriptPath->text());
#endif
    settings.sync();
    QDialog::accept();
}

void PreferenceDialog::onKeySequenceChanged(const QKeySequence &keySequence)
{
    QSettings &settings = derflaApp->settings();
    if (keySequence == QKeySequence(settings.value("hotkey", "Alt+Space").toString()))
    {
        return;
    }
}

void PreferenceDialog::onCurrentSkinChanged(const QString &name) {}

void PreferenceDialog::onAlfredStyleUIStateChanged(int)
{
    if (cbAlfredStyleUI_->isChecked())
    {
        loadThemes();
    }
    else
    {
        loadSkins();
    }
}

void PreferenceDialog::loadThemes()
{
    cbSkins_->clear();
    QSettings &settings = derflaApp->settings();
#if defined(Q_OS_MAC)
    QDir    dir(QCoreApplication::applicationDirPath() % "/../Resources/themes/");
    QString skinPath = settings.value("theme", isDarkMode() ? ":/themes/dark.derflatheme" : ":/themes/classic.derflatheme").toString();
#else
    QDir dir(QCoreApplication::applicationDirPath() % "/themes");
    QString skinPath = settings.value("theme", ":/themes/classic.derflatheme").toString();
#endif
    auto eil = dir.entryInfoList(QStringList() << "*.derflatheme", QDir::Files);
    for (const auto &fileInfo : eil)
    {
        cbSkins_->addItem(fileInfo.fileName());
    }
    int index = cbSkins_->findText(skinPath);
    if (index >= 0)
    {
        cbSkins_->setCurrentIndex(index);
    }
    else
    {
        cbSkins_->setCurrentText(QFileInfo(skinPath).fileName());
    }
}

void PreferenceDialog::loadSkins()
{
    cbSkins_->clear();
    QSettings &settings = derflaApp->settings();
#if defined(Q_OS_MAC)
    QDir dir(QCoreApplication::applicationDirPath() % "/../Resources/skins/");
#else
    QDir dir(QCoreApplication::applicationDirPath() % "/skins");
#endif
    auto eil = dir.entryInfoList(QStringList() << "*.zip", QDir::Files);
    for (const auto &fileInfo : eil)
    {
        cbSkins_->addItem(fileInfo.fileName());
    }
    if (cbSkins_->findText(QStringLiteral("derfla.zip")) < 0)
    {
        cbSkins_->insertItem(0, "derfla.zip");
    }
    QString skinPath = settings.value("skin", "derfla.zip").toString();
    int     index    = cbSkins_->findText(skinPath);
    if (index >= 0)
    {
        cbSkins_->setCurrentIndex(index);
    }
    else
    {
        cbSkins_->setCurrentText(QFileInfo(skinPath).fileName());
    }
}

void PreferenceDialog::on_btnBrowseBash_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Find Bash"),
                                                    "",
#if defined(Q_OS_WIN)
                                                    tr("Bash interpretor (bash.exe);;All files (*.*)")
#else
                                                    tr("Bash interpretor (bash);;All files (*.*)")
#endif
    );
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtBashPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnDetectBash_clicked()
{
    auto fileName = QStandardPaths::findExecutable(QStringLiteral("bash"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtBashPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnBrowseSh_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Find Sh"),
                                                    "",
#if defined(Q_OS_WIN)
                                                    tr("Sh interpretor (sh.exe);;All files (*.*)")
#else
                                                    tr("Sh interpretor (sh);;All files (*.*)")
#endif
    );
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtShPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnDetectSh_clicked()
{
    auto fileName = QStandardPaths::findExecutable(QStringLiteral("sh"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtShPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnBrowseNode_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Find Node"),
                                                    "",
#if defined(Q_OS_WIN)
                                                    tr("Node interpretor (node.exe);;All files (*.*)")
#else
                                                    tr("Node interpretor (node);;All files (*.*)")
#endif
    );
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtNodePath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnDetectNode_clicked()
{
    auto fileName = QStandardPaths::findExecutable(QStringLiteral("node"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtNodePath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnBrowseTcl_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Find Tcl"),
                                                    "",
#if defined(Q_OS_WIN)
                                                    tr("Tcl interpretor (tcl.exe);;All files (*.*)")
#else
                                                    tr("Tcl interpretor (tcl);;All files (*.*)")
#endif
    );
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtTclPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnDetectTcl_clicked()
{
    auto fileName = QStandardPaths::findExecutable(QStringLiteral("tcl"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtTclPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnBrowsePHP_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Find PHP"),
                                                    "",
#if defined(Q_OS_WIN)
                                                    tr("PHP interpretor (php.exe);;All files (*.*)")
#else
                                                    tr("PHP interpretor (php);;All files (*.*)")
#endif
    );
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtPHPPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnDetectPHP_clicked()
{
    auto fileName = QStandardPaths::findExecutable(QStringLiteral("php"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtPHPPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnBrowseRuby_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Find Ruby"),
                                                    "",
#if defined(Q_OS_WIN)
                                                    tr("Ruby interpretor (ruby.exe);;All files (*.*)")
#else
                                                    tr("Ruby interpretor (ruby);;All files (*.*)")
#endif
    );
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtRubyPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnDetectRuby_clicked()
{
    auto fileName = QStandardPaths::findExecutable(QStringLiteral("ruby"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtRubyPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnBrowsePython_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Find Python"),
                                                    "",
#if defined(Q_OS_WIN)
                                                    tr("Python interpretor (python.exe);;All files (*.*)")
#else
                                                    tr("Python interpretor (python);;All files (*.*)")
#endif
    );
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtPythonPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnDetectPython_clicked()
{
    auto fileName = QStandardPaths::findExecutable(QStringLiteral("python"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtPythonPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnBrowsePerl_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Find Perl"),
                                                    "",
#if defined(Q_OS_WIN)
                                                    tr("Perl interpretor (perl.exe);;All files (*.*)")
#else
                                                    tr("Perl interpretor (perl);;All files (*.*)")
#endif
    );
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtPerlPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnDetectPerl_clicked()
{
    auto fileName = QStandardPaths::findExecutable(QStringLiteral("perl"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtPerlPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnBrowseZsh_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Find Zsh"),
                                                    "",
#if defined(Q_OS_WIN)
                                                    tr("Zsh interpretor (zsh.exe);;All files (*.*)")
#else
                                                    tr("Zsh interpretor (zsh);;All files (*.*)")
#endif
    );
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtZshPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnDetectZsh_clicked()
{
    auto fileName = QStandardPaths::findExecutable(QStringLiteral("zsh"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtZshPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnBrowseLua_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Find Lua"),
                                                    "",
#if defined(Q_OS_WIN)
                                                    tr("Lua interpretor (lua.exe);;All files (*.*)")
#else
                                                    tr("Lua interpretor (lua);;All files (*.*)")
#endif
    );
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtLuaPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnDetectLua_clicked()
{
    auto fileName = QStandardPaths::findExecutable(QStringLiteral("lua"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtLuaPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnBrowseAppleScript_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Find AppleScript"), "", tr("AppleScript interpretor (applescript);;All files (*.*)"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtAppleScriptPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnDetectAppleScript_clicked()
{
    auto fileName = QStandardPaths::findExecutable(QStringLiteral("applescript"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtAppleScriptPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnBrowseCScript_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Find CScript"), "", tr("CScript interpretor (cscript.exe);;All files (*.*)"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtCScriptPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnDetectCScript_clicked()
{
    auto fileName = QStandardPaths::findExecutable(QStringLiteral("cscript"));

    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtCScriptPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnBrowseWScript_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Find WScript"), "", tr("WScript interpretor (wscript.exe);;All files (*.*)"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtWScriptPath->setText(QDir::toNativeSeparators(fileName));
}

void PreferenceDialog::on_btnDetectWScript_clicked()
{
    auto fileName = QStandardPaths::findExecutable(QStringLiteral("wscript"));
    if (!QFile::exists(fileName))
    {
        return;
    }
    ui->edtWScriptPath->setText(QDir::toNativeSeparators(fileName));
}
