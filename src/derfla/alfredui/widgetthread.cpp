#include "widgetthread.h"
#include "alfredwidget.h"
#include "defaulsearch.h"
#include <string>
#include <QCoreApplication>

WidgetThread::WidgetThread(QWidget* w, QWidget*):w(w)
{}
void WidgetThread::run()
{
    std::string text = (static_cast<AlfredWidget*>(w))->plainTextEdit->toPlainText().toStdString();
    defaulSearch search(text);
    auto tmp = search.getReturnVec((static_cast<AlfredWidget*>(w))->map);
    static_cast<AlfredWidget*>(w)->mutex_thread->lock();
    if (text == (static_cast<AlfredWidget*>(w))->plainTextEdit->toPlainText().toStdString())
    {
        (static_cast<AlfredWidget*>(w))->val = tmp;
        (static_cast<AlfredWidget*>(w))->plainTextEdit->val = &((static_cast<AlfredWidget*>(w))->val);
        emit shouldPaint();
    }
    static_cast<AlfredWidget*>(w)->mutex_thread->unlock();
}

SearchApp::SearchApp(QWidget* w, QWidget*):w(w)
{}

void SearchApp::run()
{
    (static_cast<AlfredWidget*>(w))->searchApp();
}
