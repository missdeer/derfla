#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "util_global.h"

struct UTILSHARED_EXPORT Directory
{
    Directory(const QString &dir, bool recursive);
    QString directory;
    bool    recursive = false;
};

#endif // DIRECTORY_H
