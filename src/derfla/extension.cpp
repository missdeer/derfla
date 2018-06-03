#include "stdafx.h"
#include "extension.h"

Extension::Extension(QObject *parent) 
	: QObject(parent)
{

}

bool Extension::load(const QString& configuration)
{
	return true;
}

bool Extension::query(const QString& prefix)
{
	return true;
}

bool Extension::run(DerflaAction* action)
{
	return true;
}
