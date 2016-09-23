#ifndef ALFREDWORKFLOWSCRIPTESCAPE_H
#define ALFREDWORKFLOWSCRIPTESCAPE_H

#include <QObject>

class ScriptEscape : public QObject
{
    Q_OBJECT
public:
    explicit ScriptEscape(int escape, QObject *parent = 0);

    bool escapeSpaces();
    bool escapeBackQuotes();
    bool escapeDoubleQuotes();
    bool escapeBrackets();
    bool escapeSemicolons();
    bool escapeDollars();
    bool escapeBackSlashes();
signals:

public slots:

private:
    int escape_;
};

#endif // ALFREDWORKFLOWSCRIPTESCAPE_H
