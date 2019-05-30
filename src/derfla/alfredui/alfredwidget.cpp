#include "stdafx.h"
#include <vector>
#include <QGraphicsDropShadowEffect>
#include <QPlainTextEdit>
#include <QDir>
#include <QDirIterator>
#include <QProcess>
#include <sstream>
#include <iostream>
#include <thread>
#include <functional>
#include <QApplication>
#include <QThread>
#include <QDesktopWidget>
#include <QClipboard>
#include <iterator>
#include <fstream>
#include "alfredwidget.h"
#include "ui_alfredwidget.h"
#include "alfredlistwidget.h"
#include "listitem.h"
#include "doublelistitem.h"
#include "derflaapp.h"
#include "alfredtheme.h"

#if defined(Q_OS_MAC)
bool isDarkMode();
#endif

static const int rowSize = 51;
static const int iconSize = 42;

AlfredWidget::AlfredWidget(QWidget *parent) :
    CommonWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    setUpTheme();

    plainTextEdit = new PlainText(ui->groupBox);
    plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));

    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(plainTextEdit->sizePolicy().hasHeightForWidth());

    plainTextEdit->setSizePolicy(sizePolicy1);
    plainTextEdit->setMinimumSize(theme->dimensions());
    plainTextEdit->setMaximumSize(QSize(2048, theme->dimensions().height())); //custom theme dimensions

    QFont fontInput;
    fontInput.setFamily(globalDefaultFontFamily);
    fontInput.setPointSize(theme->fontSize()); //custom theme font size

    plainTextEdit->setFont(fontInput);
    plainTextEdit->setFocusPolicy(Qt::StrongFocus);
    plainTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    plainTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    plainTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    plainTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->formLayout->setWidget(0, QFormLayout::SpanningRole, plainTextEdit);

    listWidget = new AlfredListWidget(ui->groupBox);
    plainTextEdit->listWidget = listWidget;
    listWidget->setObjectName(QStringLiteral("listWidget"));

    QFont fontCandidate;
    fontCandidate.setPointSize(20);

    listWidget->setFont(fontCandidate);
    listWidget->setFocusPolicy(Qt::NoFocus);

    ui->formLayout->setWidget(1, QFormLayout::SpanningRole, listWidget);

    listWidget->setStyleSheet(theme->listWidgetStylesheet()); //custom theme list widget
    listWidget->setIconSize(QSize(iconSize, iconSize));
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setMinimumHeight(1);
    listWidget->hide();

    ui->groupBox->setStyleSheet(theme->groupBoxStylesheet()); //custom theme group box

    auto *wndShadow = new QGraphicsDropShadowEffect;
    wndShadow->setBlurRadius(theme->blurRadius());
    wndShadow->setColor(theme->shadowColor());
    wndShadow->setOffset(theme->shadowOffset()); //custom theme shadow
    ui->groupBox->setGraphicsEffect(wndShadow);

    plainTextEdit->setStyleSheet(theme->plainTextEditStylesheet()); //custom theme text edit

    setAttribute(Qt::WA_TranslucentBackground,true);
#if defined(Q_OS_WIN)
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool );
#else
    setWindowFlags(Qt::FramelessWindowHint );
#endif
    plainTextEdit->setFocus();
    connect(plainTextEdit, &QPlainTextEdit::textChanged, this, &AlfredWidget::onTextChanged);
    connect(plainTextEdit, &PlainText::enterItem, this, &AlfredWidget::onEnterItem);
    connect(plainTextEdit, &PlainText::escape, this, &AlfredWidget::onEscape);
    connect(listWidget, &QListWidget::currentRowChanged, this, &AlfredWidget::setOne);
    connect(listWidget, &QListWidget::itemPressed, this, &AlfredWidget::enterCurItem);
    plainTextEdit->setFocus();
    
    connect(derflaApp, &DerflaApp::actionUpdated, this, &AlfredWidget::onActionUpdated);
    connect(derflaApp, &DerflaApp::emptyAction, this, &AlfredWidget::onEmptyAction);
    connect(this, &AlfredWidget::done, this, &AlfredWidget::onDone);
    
    setMaximumHeight(theme->beginHeight() - 6); 
    setMinimumHeight(theme->beginHeight() - 6);
    setGeometry(x(), y(), width(), theme->beginHeight());
}

void AlfredWidget::setOne()
{
    if (listWidget->currentRow() == -1) 
        listWidget->setCurrentRow(0);
}

void AlfredWidget::enterCurItem()
{
    onEnterItem(listWidget->currentRow());
}

void AlfredWidget::setUpTheme()
{
    QSettings &settings = derflaApp->settings();
    QString themePath = settings.value("theme").toString();    
    
    if (!QFile::exists(themePath))
    {
#if defined (Q_OS_MAC)
    if (isDarkMode())
        themePath = ":/themes/dark.derflatheme";
    else
#else
    themePath = ":/themes/classic.derflatheme"; 
#endif
    }
    
    theme = themeManager.applyTheme(themePath);
    
    if (!theme)
    {
#if defined (Q_OS_MAC)
    if (isDarkMode())
        themePath = ":/themes/dark.derflatheme";
    else
#else
    themePath = ":/themes/classic.derflatheme"; 
#endif
        theme = themeManager.applyTheme(themePath);
    }
    
    Q_ASSERT(theme);
}

void AlfredWidget::hideCandidateList()
{
    listWidget->clear();
    listWidget->setGeometry(listWidget->x(), theme->listWidgetY(), listWidget->width(), 0);
    setMinimumHeight(theme->beginHeight() - 6);
    setGeometry(x(), y(), width(), theme->beginHeight());
}

void AlfredWidget::onTextChanged()
{
    QString text = plainTextEdit->toPlainText();
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
    listWidget->clear();
    DerflaActionList &dal = derflaApp->derflaActions();
    if (dal.empty())
    {
        setMaximumHeight(theme->beginHeight() - 6); 
        setMinimumHeight(theme->beginHeight() - 6);
        setGeometry(x(), y(), width(), theme->beginHeight());
        return;
    }
    int size = dal.size();
    int printsize = std::min(int(dal.size()), 9);
    QString text = plainTextEdit->toPlainText();
    if (text.isEmpty())
        return;
    listWidget->show();
    listWidget->setMaximumHeight(rowSize * printsize);
    listWidget->setGeometry(listWidget->x(), theme->listWidgetY(), listWidget->width(), rowSize * printsize);

    setMaximumHeight(printsize * rowSize + theme->beginHeight());
    setMinimumHeight(printsize * rowSize + theme->beginHeight());
    setGeometry(x(), y(), width(), printsize * rowSize + theme->beginHeight());

    for (int i = 0; i < size; i++)
    {
        const DerflaActionPtr &da = dal.at(i);
        QWidget* l = nullptr;
        if (da->description().isEmpty()) 
            l = new ListItem(da->icon(), da->title(), (i < 9 ? "Alt+" : "" )+ QString::number(i + 1));
        else 
            l = new DoubleListItem(da->icon(), da->title(), da->description(), (i < 9 ? "Alt+" : "" ) + QString::number(i + 1));
        auto * item = new QListWidgetItem(listWidget);
        item->setSizeHint(QSize(l->width(), l->height()));
        listWidget->addItem(item);
        listWidget->setItemWidget(item, l);
    }
    listWidget->setCurrentRow(0);
    listWidget->setGeometry(listWidget->x(), theme->listWidgetY(), listWidget->width(), rowSize * printsize);
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
    if (index < listWidget->count() && index >= 0)
    {
        DerflaActionPtr da = derflaApp->derflaAction(index);
        if (!da->disabled())
        {
            derflaApp->executeAction(da);
            emit done();
        }
    }
}

void AlfredWidget::onEscape()
{
    if (listWidget->count())
    {
        derflaApp->clearDerflaAction();
        populateList();
        return;
    }
    
    if (!plainTextEdit->toPlainText().isEmpty())
    {
        plainTextEdit->clear();
        return;
    }
    
    hide();
}

void AlfredWidget::onDone()
{
    derflaApp->clearDerflaAction();
    plainTextEdit->clear();
    populateList();
}

void AlfredWidget::onLoadSkin()
{

}

void AlfredWidget::onStayOnTop()
{

}

void AlfredWidget::onSelectFile()
{
    if (!isVisible())
        return;
    hideCandidateList();
    QString fileName = QFileDialog::getOpenFileName(this);
    if (fileName.isEmpty())
        return;

    QString text = plainTextEdit->toPlainText();
    if (!text.isEmpty() && !text.endsWith(" "))
        text.append(" ");
    text.append(QDir::toNativeSeparators(fileName));
    plainTextEdit->setPlainText(text);
}

void AlfredWidget::onSelectFolder()
{
    if (!isVisible())
        return;

    hideCandidateList();
    QString fileName = QFileDialog::getExistingDirectory(this);
    if (fileName.isEmpty())
        return;

    QString text = plainTextEdit->toPlainText();
    if (!text.isEmpty() && !text.endsWith(" "))
        text.append(" ");
    text.append(QDir::toNativeSeparators(fileName));
    plainTextEdit->setPlainText(text);
}

bool AlfredWidget::onPreference()
{
    return CommonWidget::onPreference();
}
