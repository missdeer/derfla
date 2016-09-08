#ifndef UNIX_UTIL_H
#define UNIX_UTIL_H

struct Directory;

namespace util
{
    void processFile(const Directory& d, const QFileInfo& fileInfo);
};

#endif // UNIX_UTIL_H
