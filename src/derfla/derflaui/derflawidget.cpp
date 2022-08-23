#include "stdafx.h"

#include <private/qzipreader_p.h>

#include "derflawidget.h"
#include "candidatelist.h"
#include "charlineedit.h"
#include "derflaapp.h"
#include "extensionmanager.h"
#include "preferencedialog.h"
#include "skinmanager.h"

DerflaWidget::DerflaWidget(QWidget *parent)
    : CommonWidget(parent),
      mouseMovePos_(0, 0),
      candidateDelayTimer_(new QTimer(this)),
      input_(new CharLineEdit(this)),
      candidateList_(new CandidateList(this)),
      skinManager_(new SkinManager)
{
#if defined(Q_OS_WIN)
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
#else
    setWindowFlags(Qt::FramelessWindowHint);
#endif
    setAttribute(Qt::WA_InputMethodEnabled);
    setAttribute(Qt::WA_TranslucentBackground);

    setFocusPolicy(Qt::ClickFocus);

    setWindowIcon(QIcon(":/derfla.png"));

    candidateDelayInterval_ = derflaApp->settings_.value("interval", 0).toInt();

    QString skinPath = derflaApp->settings_.value("skin", ":/skins/derfla.zip").toString();
    if (!applySkin(skinPath))
    {
        qWarning() << "loading skin failed:" << skinPath;
        if (!applySkin(":/skins/derfla.zip"))
        {
            qCritical() << "loading skin failed";
            return;
        }
    }
#ifdef Q_WS_MAC
    QMacStyle::setFocusRectPolicy(input, QMacStyle::FocusDisabled);
#endif

    connect(candidateList_, &CandidateList::done, this, &DerflaWidget::onCandidateListDone);
    connect(candidateList_, &CandidateList::keyPressedEvent, this, &DerflaWidget::onCandidateListKeyPressed);
    //    QAction *logoAction = new QAction(tr("Input"), this);
    //    logoAction->setIcon(QIcon(":/derfla.ico"));
    //    input_->addAction(logoAction, QLineEdit::ActionPosition::TrailingPosition);
    input_->setObjectName("input");
    input_->setClearButtonEnabled(false);
    connect(input_, &CharLineEdit::keyPressed, this, &DerflaWidget::keyPressEvent);
    connect(input_, &QLineEdit::textChanged, this, &DerflaWidget::onInputChanged);
    connect(input_, &CharLineEdit::focusOut, this, &DerflaWidget::onInputFocusOut);
    connect(input_, &CharLineEdit::focusIn, this, &DerflaWidget::onInputFocusIn);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this, &DerflaWidget::onCustomContextMenuRequested);
    connect(candidateDelayTimer_, &QTimer::timeout, this, &DerflaWidget::onCandidateDelayTimer);
    candidateDelayTimer_->setSingleShot(true);
}

DerflaWidget::~DerflaWidget()
{
    candidateDelayTimer_->stop();
    delete skinManager_;
}

void DerflaWidget::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton))
    {
        move(mapToParent(event->pos() - mouseMovePos_));
    }
}

void DerflaWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mouseMovePos_ = event->pos();
    }
}

void DerflaWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mouseMovePos_ = QPoint(0, 0);
    }
}

void DerflaWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption styleOption;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    styleOption.init(this);
#else
#endif
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    style()->drawPrimitive(QStyle::PE_Widget, &styleOption, &painter, this);
    QSize size(skinManager_->backgroundImage().size());

    if (size.width() > widgetMinWidth_)
    {
        painter.drawPixmap(0, 0, skinManager_->backgroundImage());
    }
    else
    {
        painter.drawPixmap(0, 0, skinManager_->leftPartBackgroundImage());
        painter.drawPixmap(skinManager_->leftPartBackgroundImage().width(), 0, skinManager_->midPartBackgroundImage());
        painter.drawPixmap(skinManager_->leftPartBackgroundImage().width() + skinManager_->midPartBackgroundImage().width(),
                           0,
                           skinManager_->rightPartBackgroundImage());
    }
    QWidget::paintEvent(event);
}

void DerflaWidget::moveEvent(QMoveEvent * /*event*/)
{
    if (candidateList_ && candidateList_->isVisible())
        candidateList_->move(mapToGlobal(QPoint(input_->x(), input_->y() + input_->height())));
}

void DerflaWidget::keyPressEvent(QKeyEvent *event)
{
    // qDebug() << "DerflaWidget::keyPressEvent 0:" << event->key();
    if (event->key() == Qt::Key_Escape)
    {
        // qDebug() << "DerflaWidget::keyPressEvent escape" << candidateList_->isVisible();
        if (candidateList_->isVisible())
            candidateList_->hide();
        else
        {
            if (input_->text().isEmpty() && !stayOnTop_)
                hide();
            else
                input_->setText("");
        }
    }
    else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        doEnter();
    }
    else if (event->key() == Qt::Key_Down || event->key() == Qt::Key_PageDown || event->key() == Qt::Key_Up || event->key() == Qt::Key_PageUp)
    {
        // qDebug() << "DerflaWidget::keyPressEvent 1:" << event->key();
        if (candidateList_->isVisible())
        {
            // qDebug() << "DerflaWidget::keyPressEvent 2:" << event->key();
            candidateList_->activateWindow();
            candidateList_->setActiveWindowFlag(true);
            QCoreApplication::instance()->sendEvent(candidateList_, event);
        }
    }
    else if ((event->key() == Qt::Key_Tab || event->key() == Qt::Key_Backspace) && event->modifiers() == Qt::ShiftModifier)
    {
        doBackTab();
        processKey();
    }
    else if (event->key() == Qt::Key_Tab)
    {
        doTab();
        processKey();
    }
    else if (event->key() == Qt::Key_Slash || event->key() == Qt::Key_Backslash)
    {
        //        if (inputData.count() > 0 && inputData.last().hasLabel(LABEL_FILE) &&
        //                searchResults.count() > 0 && searchResults[0].id == HASH_LAUNCHYFILE)
        //            doTab();
        processKey();
    }
    else if (event->key() == Qt::Key_Insert && event->modifiers() == Qt::ShiftModifier)
    {
        // ensure pasting text with Shift+Insert also parses input
        // longer term parsing should be done using the TextChanged event
        processKey();
    }
    else if (event->text().length() > 0)
    {
        if (!input_->hasFocus())
        {
            input_->setFocus();
            QCoreApplication::instance()->sendEvent(input_, event);
        }
        // process any other key with character output
        event->ignore();
        processKey();
    }
}

void DerflaWidget::focusOutEvent(QFocusEvent *)
{
    if (!input_->hasFocus() && candidateList_->isVisible() && !candidateList_->isActiveWindow())
        hideCandidateList();
}

void DerflaWidget::focusInEvent(QFocusEvent *e)
{
    onInputFocusIn(e);
}

void DerflaWidget::onInputChanged(const QString &text)
{
    // qDebug() <<  "DerflaWidget::inputChanged:" << input_->text();
    if (text.isEmpty())
    {
        hideCandidateList();
        stopWaiting();
        candidateDelayTimer_->stop();
    }
    else
    {
        candidateDelayTimer_->start(candidateDelayInterval_);
    }
}

void DerflaWidget::onCandidateListKeyPressed(QKeyEvent *e)
{
    // qDebug() << "DerflaWidget::keyPressed" << e;
    //    if ( e->key() != Qt::Key_Escape)
    //        hideCandidateList();
    activateWindow();
    raise();
    QCoreApplication::instance()->sendEvent(this, e);
}

void DerflaWidget::onInputFocusOut(QFocusEvent *)
{
    // input lost focus
    if (!hasFocus() && candidateList_->isVisible() && !candidateList_->isActiveWindow())
        hideCandidateList();
}

void DerflaWidget::onInputFocusIn(QFocusEvent *)
{
    // input got focus
    if (!candidateList_->isVisible() && !input_->text().isEmpty())
    {
        candidateDelayTimer_->start(candidateDelayInterval_);
    }
}

void DerflaWidget::onLoadSkin()
{
    CommonWidget::onLoadSkin();

    hideCandidateList();
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Load Derfla Skin"),
                                                    "",
                                                    tr("Derfla Skin Package (*.zip);;"
                                                       "Derfla Skin Configuration (*.derflaskin);;"
                                                       "All Supported Files (*.derflaskin *.zip);;"
                                                       "All files (*.*)"));
    if (!QFile::exists(fileName))
        return;
    if (applySkin(fileName))
    {
        derflaApp->settings_.setValue("skin", fileName);
        derflaApp->settings_.sync();
    }
}

void DerflaWidget::onStayOnTop()
{
    auto *action = qobject_cast<QAction *>(sender());
    stayOnTop_   = !stayOnTop_;
    action->setChecked(stayOnTop_);
    CommonWidget::onStayOnTop();
}

void DerflaWidget::onCandidateListDone()
{
    hideCandidateList();
    input_->setText("");
}

void DerflaWidget::onCandidateDelayTimer()
{
    showCandidateList();
}

void DerflaWidget::onSelectFile()
{
    if (!isVisible())
        return;
    hideCandidateList();
    QString fileName = QFileDialog::getOpenFileName(this);
    if (fileName.isEmpty())
        return;

    QString text = input_->text();
    if (!text.isEmpty() && !text.endsWith(" "))
        text.append(" ");
    text.append(QDir::toNativeSeparators(fileName));
    input_->setText(text);
}

void DerflaWidget::onSelectFolder()
{
    if (!isVisible())
        return;

    hideCandidateList();
    QString fileName = QFileDialog::getExistingDirectory(this);
    if (fileName.isEmpty())
        return;

    QString text = input_->text();
    if (!text.isEmpty() && !text.endsWith(" "))
        text.append(" ");
    text.append(QDir::toNativeSeparators(fileName));
    input_->setText(text);
}

bool DerflaWidget::onPreference()
{
    if (!CommonWidget::onPreference())
        return false;

    candidateDelayInterval_ = derflaApp->settings_.value("interval", 0).toInt();

    QString skinPath = derflaApp->settings_.value("skin", ":/skins/derfla.zip").toString();
    if (!applySkin(skinPath))
    {
        qWarning() << "loading skin failed:" << skinPath;
        if (!applySkin(":/skins/derfla.zip"))
        {
            qCritical() << "loading skin failed";
            return false;
        }
    }

    return true;
}

void DerflaWidget::onCustomContextMenuRequested(const QPoint &pos)
{
    Q_ASSERT(derflaApp && derflaApp->trayIcon_);
    auto menu = derflaApp->trayIcon_->contextMenu();
    Q_ASSERT(menu);
    menu->exec(mapToGlobal(pos));
}

void DerflaWidget::showCandidateList()
{
    if (hasFocus() || isActiveWindow() || input_->hasFocus())
    {
        candidateList_->move(mapToGlobal(QPoint(input_->x(), input_->y() + input_->height())));

        QString inputText = input_->text().trimmed();

        candidateList_->update(inputText);
    }
}

void DerflaWidget::processKey()
{
    // qDebug() << "DerflaWidget::processKey";
}

void DerflaWidget::doEnter()
{
    qDebug() << "DerflaWidget::doEnter";
    if (candidateList_->isVisible())
        candidateList_->onEnter();
}

void DerflaWidget::doTab()
{
    // qDebug() << "DerflaWidget::doTab";
}

void DerflaWidget::doBackTab()
{
    // qDebug() << "DerflaWidget::doBackTab";
}

bool DerflaWidget::applySkin(const QString &skin)
{
    if (!skinManager_->applySkin(skin))
        return false;

    resize(1, 1);
    resize(skinManager_->skinSize());

    input_->setStyleSheet(skinManager_->inputStyle());
    QFont f = input_->font();
    f.setFamily(globalDefaultFontFamily);
    input_->setFont(f);

    candidateList_->setInputBoxSize(input_->size());

    return true;
}

void DerflaWidget::hideCandidateList()
{
    if (candidateList_->isVisible())
        candidateList_->hide();
}

void DerflaWidget::stopWaiting()
{
    QList<QAction *> actions = input_->actions();
    if (actions.isEmpty())
        return;
    QAction *logoAction = actions.at(0);
    logoAction->setIcon(QIcon(":/derfla.ico"));
}
