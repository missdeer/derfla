#ifndef PLAINTEXT_H
#define PLAINTEXT_H
#include <QListWidget>
#include <QPlainTextEdit>
#include <QWidget>

class PlainText : public QPlainTextEdit
{
    Q_OBJECT
signals:
    void escape();
    void enterItem(int index);

public:
    QListWidget *listWidget;
    explicit PlainText(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // PLAINTEXT_H
