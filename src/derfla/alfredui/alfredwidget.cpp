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

#define rowsize 51
#define iconsize 42

AlfredWidget::AlfredWidget(QWidget *parent) :
    CommonWidget(parent),
    ui(new Ui::Widget),
    mutex_thread(new QMutex)
{
    ui->setupUi(this);

    setUpTheme();

    plainTextEdit = new PlainText(ui->groupBox);
    plainTextEdit->parent = this;
    plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));

    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(plainTextEdit->sizePolicy().hasHeightForWidth());

    plainTextEdit->setSizePolicy(sizePolicy1);
    plainTextEdit->setMinimumSize(theme->dimensions());
    plainTextEdit->setMaximumSize(QSize(2048, theme->dimensions().height())); //custom theme dimensions

    QFont font2;
    font2.setPointSize(theme->fontSize()); //custom theme font size

    plainTextEdit->setFont(font2);
    plainTextEdit->setFocusPolicy(Qt::StrongFocus);
    plainTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    plainTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    plainTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    plainTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->formLayout->setWidget(0, QFormLayout::SpanningRole, plainTextEdit);

    listWidget = new AlfredListWidget(ui->groupBox);
    plainTextEdit->listWidget = listWidget;
    listWidget->setObjectName(QStringLiteral("listWidget"));

    QFont font1;
    font1.setPointSize(20);

    listWidget->setFont(font1);
    listWidget->setFocusPolicy(Qt::NoFocus);

    ui->formLayout->setWidget(1, QFormLayout::SpanningRole, listWidget);

    listWidget->setStyleSheet(theme->listWidgetStylesheet()); //custom theme list widget
    listWidget->setIconSize(QSize(iconsize, iconsize));
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setMinimumHeight(1024);
    listWidget->hide();

    ui->groupBox->setStyleSheet(theme->groupBoxStylesheet()); //custom theme group box

    QGraphicsDropShadowEffect *wndShadow = new QGraphicsDropShadowEffect;
    wndShadow->setBlurRadius(theme->blurRadius());
    wndShadow->setColor(theme->shadowColor());
    wndShadow->setOffset(theme->shadowOffset()); //custom theme shadow
    ui->groupBox->setGraphicsEffect(wndShadow);

    plainTextEdit->setStyleSheet(theme->plainTextEditStylesheet()); //custom theme text edit

    setAttribute(Qt::WA_TranslucentBackground,true);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    plainTextEdit->setFocus();
    connect(plainTextEdit, &QPlainTextEdit::textChanged, this, &AlfredWidget::onTextChanged);
    connect(listWidget, &QListWidget::currentRowChanged, this, &AlfredWidget::setOne);
    connect(listWidget, &QListWidget::itemPressed, this, &AlfredWidget::enterCurItem);
    plainTextEdit->setFocus();
    
    connect(derflaApp, &DerflaApp::actionUpdated, this, &AlfredWidget::onActionUpdated);
    connect(derflaApp, &DerflaApp::emptyAction, this, &AlfredWidget::onEmptyAction);
}

void AlfredWidget::setOne()
{
    if (listWidget->currentRow() == -1) listWidget->setCurrentRow(0);
}

void AlfredWidget::enterCurItem()
{
    plainTextEdit->enterCurrentRow();
}

/**
 * @brief Widget::setUpTheme
 * Function reads from file "theme.ini" the theme and applies it.
 * Two themes are supported: classic and dark.
 */
void AlfredWidget::setUpTheme()
{
    std::string themeName = "Classic";
    
#if defined (Q_OS_MAC)
    if (isDarkMode())
        themeName = "Dark";
#endif
    
    theme = new AlfredTheme();
    
    int globalBeginHeight;
    int globalListWidgetY;
    QSize dimensions;
    QString groupBoxStylesheet;
    QString plainTextEditStylesheet;
    QString listWidgetStylesheet;
    int fontSize;
    qreal blurRadius;
    QColor shadowColor;
    qreal shadowOffset;

    if(themeName == "Classic")
    {
        globalBeginHeight = 125;
        globalListWidgetY = 91;
        dimensions = QSize(550, 75);
        groupBoxStylesheet = "QGroupBox{background:white;border-radius: 9px;}";
        plainTextEditStylesheet = "QPlainTextEdit{border: 1px solid white}";
        listWidgetStylesheet = "QListWidget{border: 1px solid white} QListWidget::item{padding : 3px 3px 3px 3px}";
        fontSize = 40;
        blurRadius = 9.0;
        shadowColor = QColor(0, 0, 0, 160);
        shadowOffset = 3.0;
    }
    else
    {
        globalBeginHeight = 94;
        globalListWidgetY = 65;
        dimensions = QSize(550, 50);
        groupBoxStylesheet = "QGroupBox{background:#444444;border-radius: 9px; padding: -3px -3px -3px -3px;}";
        plainTextEditStylesheet = "QPlainTextEdit{background:#3a3a3a; border: 1px solid #3a3a3a; color: #AAAAAA}";
        listWidgetStylesheet = "QListWidget { background: #444444; border: 1px solid #444444}"
                               "QListWidget::item { background: #444444; padding: 0px 0px 0px 3px;}"
                               "QListWidget::item:selected { background: #525252; border: 1px solid #AAAAAA; border-radius: 2px;}"
                               "QLabel {color: #AAAAAA;}";
        fontSize = 30;
        blurRadius = 10.0;
        shadowColor = QColor(0, 0, 0, 200);
        shadowOffset = 0.0;
    }

    theme->setBeginHeight(globalBeginHeight);
    theme->setListWidgetY(globalListWidgetY);
    theme->setDimensions(dimensions);
    theme->setGroupBoxStylesheet(groupBoxStylesheet);
    theme->setPlainTextEditStylesheet(plainTextEditStylesheet);
    theme->setListWidgetStylesheet(listWidgetStylesheet);
    theme->setFontSize(fontSize);
    theme->setBlurRadius(blurRadius);
    theme->setShadowColor(shadowColor);
    theme->setShadowOffset(shadowOffset);
}

void AlfredWidget::onTextChanged()
{
    QString text = plainTextEdit->toPlainText();
    derflaApp->clearDerflaAction();
    derflaApp->queryByExtension(text);
}

void AlfredWidget::populateList()
{
    listWidget->clear();
    DerflaActionList &dal = derflaApp->derflaActions();
    int size = std::min(int(dal.size()), 9);
    int printsize = std::min(int(dal.size()), 9);
    if (dal.empty())
    {
        listWidget->hide();
        setMaximumHeight(printsize * rowsize + theme->beginHeight() - 6); //custom theme begin height
        setMinimumHeight(printsize * rowsize + theme->beginHeight() - 6);
        setGeometry(x(), y(), dal.size() * rowsize + theme->beginHeight(), width());
        return;
    }
    listWidget->show();
    listWidget->setMaximumHeight(rowsize * printsize);
    listWidget->setGeometry(listWidget->x(), theme->listWidgetY(), listWidget->width(), rowsize * printsize);

    setMaximumHeight(printsize * rowsize + theme->beginHeight());
    setMinimumHeight(printsize * rowsize + theme->beginHeight());
    setGeometry(x(), y(), printsize * rowsize + theme->beginHeight(), width());

    for (size_t i = 0; i < size; i++)
    {
        DerflaActionPtr da = dal.at(i);
        QWidget* l;
        if (da->description().isEmpty()) {
            if (i < 9) {
                l = new ListItem(da->icon(), da->title(), "Alt+" + QString::number(i + 1));
            }
            else {
                l = new ListItem(da->icon(), da->title(), QString::number(i + 1));
            }
        }
        else {
            if (i < 9)
            {
                l = new DoubleListItem(da->icon(), da->title(), da->description(), "Alt+" + QString::number(i + 1));
            }
            else {
                l = new ListItem(da->icon(), da->title(), QString::number(i + 1));
            }
        }
        QListWidgetItem * item = new QListWidgetItem(listWidget);
        item->setSizeHint(QSize(l->width(), l->height()));
        listWidget->addItem(item);
        listWidget->setItemWidget(item, l);
    }
    listWidget->setCurrentRow(0);
    listWidget->setGeometry(listWidget->x(), theme->listWidgetY(), listWidget->width(), rowsize * printsize);
    derflaApp->centerToScreen(this);
}
#undef signals

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
