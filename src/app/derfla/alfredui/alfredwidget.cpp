#include "stdafx.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <thread>
#include <vector>

#include <QApplication>
#include <QClipboard>
#include <QDir>
#include <QDirIterator>
#include <QGraphicsDropShadowEffect>
#include <QPlainTextEdit>
#include <QProcess>
#include <QThread>

#include "alfredwidget.h"
#include "alfredlistwidget.h"
#include "alfredtheme.h"
#include "derflaapp.h"
#include "doublelistitem.h"
#include "listitem.h"
#include "ui_alfredwidget.h"

#if defined(Q_OS_MAC)
#    include "darkmode.h"
#endif

static const int rowSize  = 51;
static const int iconSize = 42;

AlfredWidget::AlfredWidget(QWidget *parent) : CommonWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    setUpTheme();

    m_plainTextEdit = new PlainText(ui->groupBox);
    m_plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));

    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(m_plainTextEdit->sizePolicy().hasHeightForWidth());

    m_plainTextEdit->setSizePolicy(sizePolicy1);
    m_plainTextEdit->setMinimumSize(m_theme->dimensions());
    m_plainTextEdit->setMaximumSize(QSize(2048, m_theme->dimensions().height())); // custom theme dimensions

    QFont fontInput;
    fontInput.setFamily(globalDefaultFontFamily);
    fontInput.setPointSize(m_theme->fontSize()); // custom theme font size

    m_plainTextEdit->setFont(fontInput);
    m_plainTextEdit->setFocusPolicy(Qt::StrongFocus);
    m_plainTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_plainTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_plainTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_plainTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->formLayout->setWidget(0, QFormLayout::SpanningRole, m_plainTextEdit);

    m_listWidget                = new AlfredListWidget(ui->groupBox);
    m_plainTextEdit->listWidget = m_listWidget;
    m_listWidget->setObjectName(QStringLiteral("listWidget"));

    QFont fontCandidate;
    fontCandidate.setPointSize(20);

    m_listWidget->setFont(fontCandidate);
    m_listWidget->setFocusPolicy(Qt::NoFocus);

    ui->formLayout->setWidget(1, QFormLayout::SpanningRole, m_listWidget);

    m_listWidget->setStyleSheet(m_theme->listWidgetStylesheet()); // custom theme list widget
    m_listWidget->setIconSize(QSize(iconSize, iconSize));
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setMinimumHeight(1);
    m_listWidget->hide();

    ui->groupBox->setStyleSheet(m_theme->groupBoxStylesheet()); // custom theme group box

    auto *wndShadow = new QGraphicsDropShadowEffect;
    wndShadow->setBlurRadius(m_theme->blurRadius());
    wndShadow->setColor(m_theme->shadowColor());
    wndShadow->setOffset(m_theme->shadowOffset()); // custom theme shadow
    ui->groupBox->setGraphicsEffect(wndShadow);

    m_plainTextEdit->setStyleSheet(m_theme->plainTextEditStylesheet()); // custom theme text edit

    setAttribute(Qt::WA_TranslucentBackground, true);
#if defined(Q_OS_WIN)
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
#else
    setWindowFlags(Qt::FramelessWindowHint);
#endif
    m_plainTextEdit->setFocus();
    connect(m_plainTextEdit, &QPlainTextEdit::textChanged, this, &AlfredWidget::onTextChanged);
    connect(m_plainTextEdit, &PlainText::enterItem, this, &AlfredWidget::onEnterItem);
    connect(m_plainTextEdit, &PlainText::escape, this, &AlfredWidget::onEscape);
    connect(m_listWidget, &QListWidget::currentRowChanged, this, &AlfredWidget::setOne);
    connect(m_listWidget, &QListWidget::itemPressed, this, &AlfredWidget::enterCurItem);
    m_plainTextEdit->setFocus();

    connect(derflaApp, &DerflaApp::actionUpdated, this, &AlfredWidget::onActionUpdated);
    connect(derflaApp, &DerflaApp::emptyAction, this, &AlfredWidget::onEmptyAction);
    connect(this, &AlfredWidget::done, this, &AlfredWidget::onDone);

    setMaximumHeight(m_theme->beginHeight() - 6);
    setMinimumHeight(m_theme->beginHeight() - 6);
    setGeometry(x(), y(), width(), m_theme->beginHeight());

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &AlfredWidget::onCustomContextMenuRequested);
}

void AlfredWidget::onCustomContextMenuRequested(const QPoint &pos)
{
    Q_ASSERT(derflaApp && derflaApp->trayIcon_);
    auto *menu = derflaApp->trayIcon_->contextMenu();
    Q_ASSERT(menu);
    menu->exec(mapToGlobal(pos));
}

void AlfredWidget::setOne()
{
    if (m_listWidget->currentRow() == -1)
    {
        m_listWidget->setCurrentRow(0);
    }
}

void AlfredWidget::enterCurItem()
{
    onEnterItem(m_listWidget->currentRow());
}

void AlfredWidget::setUpTheme()
{
    QSettings &settings  = derflaApp->settings();
    QString    themePath = settings.value("theme").toString();

    if (!QFile::exists(themePath))
    {
#if defined(Q_OS_MAC)
        if (isDarkMode())
            themePath = ":/themes/dark.derflatheme";
        else
#endif
            themePath = ":/themes/classic.derflatheme";
    }

    m_theme = m_themeManager.loadThemeFromFile(themePath);

    if (!m_theme)
    {
#if defined(Q_OS_MAC)
        if (isDarkMode())
            themePath = ":/themes/dark.derflatheme";
        else
#endif
            themePath = ":/themes/classic.derflatheme";
        m_theme = m_themeManager.loadThemeFromFile(themePath);
    }

    Q_ASSERT(m_theme);
}

void AlfredWidget::hideCandidateList()
{
    m_listWidget->clear();
    m_listWidget->setGeometry(m_listWidget->x(), m_theme->listWidgetY(), m_listWidget->width(), 0);
    setMinimumHeight(m_theme->beginHeight() - 6);
    setGeometry(x(), y(), width(), m_theme->beginHeight());
}

void AlfredWidget::onTextChanged()
{
    QString text = m_plainTextEdit->toPlainText();
    derflaApp->clearDerflaAction();
    if (text.isEmpty())
    {
        hideCandidateList();
    }
    else
        derflaApp->queryByExtension(text);
}

void AlfredWidget::populateList()
{
    m_listWidget->clear();
    DerflaActionList &dal = derflaApp->derflaActions();
    if (dal.empty())
    {
        setMaximumHeight(m_theme->beginHeight() - 6);
        setMinimumHeight(m_theme->beginHeight() - 6);
        setGeometry(x(), y(), width(), m_theme->beginHeight());
        return;
    }
    int     size      = dal.size();
    int     printsize = std::min(int(dal.size()), 9);
    QString text      = m_plainTextEdit->toPlainText();
    if (text.isEmpty())
    {
        return;
    }
    m_listWidget->show();
    m_listWidget->setMaximumHeight(rowSize * printsize);
    m_listWidget->setGeometry(m_listWidget->x(), m_theme->listWidgetY(), m_listWidget->width(), rowSize * printsize);

    setMaximumHeight(printsize * rowSize + m_theme->beginHeight());
    setMinimumHeight(printsize * rowSize + m_theme->beginHeight());
    setGeometry(x(), y(), width(), printsize * rowSize + m_theme->beginHeight());

    for (int i = 0; i < size; i++)
    {
        const DerflaActionPtr &action = dal.at(i);
        QWidget               *widget = nullptr;
        if (action->description().isEmpty())
        {
            widget = new ListItem(action->icon(), action->title(), (i < 9 ? "Alt+" : "") + QString::number(i + 1));
        }
        else
        {
            widget = new DoubleListItem(action->icon(), action->title(), action->description(), (i < 9 ? "Alt+" : "") + QString::number(i + 1));
        }
        auto *item = new QListWidgetItem(m_listWidget);
        item->setSizeHint(QSize(widget->width(), widget->height()));
        m_listWidget->addItem(item);
        m_listWidget->setItemWidget(item, widget);
    }
    m_listWidget->setCurrentRow(0);
    m_listWidget->setGeometry(m_listWidget->x(), m_theme->listWidgetY(), m_listWidget->width(), rowSize * printsize);
}

AlfredWidget::~AlfredWidget()
{
    delete ui;
}

void AlfredWidget::onActionUpdated()
{
    populateList();
}

void AlfredWidget::onEmptyAction()
{
    populateList();
}

void AlfredWidget::onEnterItem(int index)
{
    if (index < m_listWidget->count() && index >= 0)
    {
        DerflaActionPtr action = derflaApp->derflaAction(index);
        if (!action->disabled())
        {
            derflaApp->executeAction(action);
            emit done();
        }
    }
}

void AlfredWidget::onEscape()
{
    if (m_listWidget->count())
    {
        derflaApp->clearDerflaAction();
        populateList();
        return;
    }

    if (!m_plainTextEdit->toPlainText().isEmpty())
    {
        m_plainTextEdit->clear();
        return;
    }

    hide();
}

void AlfredWidget::onDone()
{
    derflaApp->clearDerflaAction();
    m_plainTextEdit->clear();
    populateList();
}

void AlfredWidget::onLoadSkin() {}

void AlfredWidget::onStayOnTop() {}

void AlfredWidget::onSelectFile()
{
    if (!isVisible())
    {
        return;
    }
    hideCandidateList();
    QString fileName = QFileDialog::getOpenFileName(this);
    if (fileName.isEmpty())
    {
        return;
    }

    QString text = m_plainTextEdit->toPlainText();
    if (!text.isEmpty() && !text.endsWith(" "))
    {
        text.append(" ");
    }
    text.append(QDir::toNativeSeparators(fileName));
    m_plainTextEdit->setPlainText(text);
}

void AlfredWidget::onSelectFolder()
{
    if (!isVisible())
    {
        return;
    }

    hideCandidateList();
    QString fileName = QFileDialog::getExistingDirectory(this);
    if (fileName.isEmpty())
    {
        return;
    }

    QString text = m_plainTextEdit->toPlainText();
    if (!text.isEmpty() && !text.endsWith(" "))
    {
        text.append(" ");
    }
    text.append(QDir::toNativeSeparators(fileName));
    m_plainTextEdit->setPlainText(text);
}

bool AlfredWidget::onPreference()
{
    return CommonWidget::onPreference();
}
