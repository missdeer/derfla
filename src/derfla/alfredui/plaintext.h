#ifndef PLAINTEXT_H
#define PLAINTEXT_H
#include <QPlainTextEdit>
#include <QListWidget>
#include <QWidget>

class PlainText : public QPlainTextEdit
{
    Q_OBJECT
public:
    QWidget *parent;
    QListWidget *listWidget;
    explicit PlainText(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent* event);
    void enterListWidget(int row);
    void focusOutEvent(QFocusEvent *event);
    void enterCurrentRow();
};

#endif // PLAINTEXT_H
