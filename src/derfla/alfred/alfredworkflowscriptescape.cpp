#include "stdafx.h"
#include "alfredworkflowscriptescape.h"

static const int escape_spaces = 1;
static const int escape_backquotes = 2;
static const int escape_double_quotes = 4;
static const int escape_brackets = 8;
static const int escape_semicolons = 16;
static const int escape_dollars = 32;
static const int escape_backslashes = 64;

AlfredWorkflowScriptEscape::AlfredWorkflowScriptEscape(QObject *parent) : QObject(parent)
{

}
