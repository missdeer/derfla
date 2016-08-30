#include "stdafx.h"
#include "alfredworkflowscriptescape.h"

static const int escape_spaces = 1;
static const int escape_back_quotes = 2;
static const int escape_double_quotes = 4;
static const int escape_brackets = 8;
static const int escape_semicolons = 16;
static const int escape_dollars = 32;
static const int escape_back_slashes = 64;

AlfredWorkflowScriptEscape::AlfredWorkflowScriptEscape(QObject *parent) : QObject(parent)
{

}

bool AlfredWorkflowScriptEscape::escapeSpaces(int val)
{
    return (val & escape_spaces);
}

bool AlfredWorkflowScriptEscape::escapeBackQuotes(int val)
{
    return (val & escape_back_quotes);
}

bool AlfredWorkflowScriptEscape::escapeDoubleQuotes(int val)
{
    return (val & escape_double_quotes);
}

bool AlfredWorkflowScriptEscape::escapeBrackets(int val)
{
    return (val & escape_brackets);
}

bool AlfredWorkflowScriptEscape::escapeSemicolons(int val)
{
    return (val & escape_semicolons);
}

bool AlfredWorkflowScriptEscape::escapeDollars(int val)
{
    return (val & escape_dollars);
}

bool AlfredWorkflowScriptEscape::escapeBackSlashes(int val)
{
    return (val & escape_back_slashes);
}
