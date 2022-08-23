#include <memory>

#include <QClipboard>
#include <QProcess>
#include <QTextEdit>
#include <QtCore>

#include "plaintext.h"

using namespace std;

PlainText::PlainText(QWidget *parent) : QPlainTextEdit(parent) {}

void PlainText::keyPressEvent(QKeyEvent *event)
{
    Qt::KeyboardModifiers modifiers = event->modifiers();
    int                   uKey      = event->key();
    auto                  key       = static_cast<Qt::Key>(uKey);

    if (modifiers & Qt::AltModifier)
    {
        if (key >= Qt::Key_1 && key <= Qt::Key_1 + std::min(this->listWidget->count(), 9) - 1)
        {
            emit enterItem(key - Qt::Key_1);
        }
        return;
    }
    if (key == Qt::Key_Escape)
    {
        emit escape();
        return;
    }
    if (key == Qt::Key_Return || key == Qt::Key_Enter)
    {
        emit enterItem(listWidget->currentRow());
        return;
    }
    if (listWidget->count() > 0)
    {
        if (key == Qt::Key_Up)
        {
            listWidget->setCurrentRow((listWidget->currentRow() + listWidget->count() - 1) % listWidget->count());
            return;
        }
        if (key == Qt::Key_Down)
        {
            listWidget->setCurrentRow((listWidget->currentRow() + 1) % listWidget->count());
            return;
        }
    }
    if (modifiers & Qt::MetaModifier)
        return;
    if (modifiers & Qt::ControlModifier)
    {
        if (key != Qt::Key_C && key != Qt::Key_V && key != Qt::Key_A && key != Qt::Key_X)
        {
            return;
        }
    }
    QPlainTextEdit::keyPressEvent(event);
}
