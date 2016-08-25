#include "stdafx.h"
#include "alfredworkflowscriptescape.h"

const int escape_spaces = 1;
const int escape_backquotes = 2;
const int escape_double_quotes = 4;
const int escape_brackets = 8;
const int escape_semicolons = 16;
const int escape_dollars = 32;
const int escape_backslashes = 64;

AlfredWorkflowScriptEscape::AlfredWorkflowScriptEscape(QObject *parent) : QObject(parent)
{

}
