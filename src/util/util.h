#ifndef UTIL_H
#define UTIL_H

#include "util_global.h"

namespace util
{
    extern UTILSHARED_EXPORT qint64 timestamp;
    UTILSHARED_EXPORT QByteArray extractPNGFromIcon(const QString& filePath);
    UTILSHARED_EXPORT QByteArray extractPNGIconFromFile(const QFileInfo& fi);
    UTILSHARED_EXPORT QStringList &getEnvPaths();
    UTILSHARED_EXPORT QStringList &getLocaleNames();
};

#endif // UTIL_H
