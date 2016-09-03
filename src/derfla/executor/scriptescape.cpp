#include "stdafx.h"
#include "scriptescape.h"

static const int escape_spaces = 1;
static const int escape_back_quotes = 2;
static const int escape_double_quotes = 4;
static const int escape_brackets = 8;
static const int escape_semicolons = 16;
static const int escape_dollars = 32;
static const int escape_back_slashes = 64;

ScriptEscape::ScriptEscape(int escape, QObject *parent)
    : QObject(parent)
    , escape_(escape)
{

}

bool ScriptEscape::escapeSpaces()
{
    return (escape_ & escape_spaces);
}

bool ScriptEscape::escapeBackQuotes()
{
    return (escape_ & escape_back_quotes);
}

bool ScriptEscape::escapeDoubleQuotes()
{
    return (escape_ & escape_double_quotes);
}

bool ScriptEscape::escapeBrackets()
{
    return (escape_ & escape_brackets);
}

bool ScriptEscape::escapeSemicolons()
{
    return (escape_ & escape_semicolons);
}

bool ScriptEscape::escapeDollars()
{
    return (escape_ & escape_dollars);
}

bool ScriptEscape::escapeBackSlashes()
{
    return (escape_ & escape_back_slashes);
}
