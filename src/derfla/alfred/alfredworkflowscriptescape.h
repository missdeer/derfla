#ifndef ALFREDWORKFLOWSCRIPTESCAPE_H
#define ALFREDWORKFLOWSCRIPTESCAPE_H

#include <QObject>

class AlfredWorkflowScriptEscape : public QObject
{
    Q_OBJECT
public:
    explicit AlfredWorkflowScriptEscape(QObject *parent = 0);

    static bool escapeSpaces(int val);
    static bool escapeBackQuotes(int val);
    static bool escapeDoubleQuotes(int val);
    static bool escapeBrackets(int val);
    static bool escapeSemicolons(int val);
    static bool escapeDollars(int val);
    static bool escapeBackSlashes(int val);
signals:

public slots:
};

#endif // ALFREDWORKFLOWSCRIPTESCAPE_H
