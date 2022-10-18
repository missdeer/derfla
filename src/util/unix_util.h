#ifndef UNIX_UTIL_H
#define UNIX_UTIL_H

#include <functional>

#include <QFileInfo>

#include "util_global.h"

struct Directory;

using LFSInserter =
    std::function<bool(const QByteArray &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &)>;

namespace util
{
    UTILSHARED_EXPORT void processFile(const Directory &d, const QFileInfo &fileInfo, LFSInserter inserter);
};

#endif // UNIX_UTIL_H
