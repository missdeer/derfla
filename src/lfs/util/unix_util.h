#ifndef UNIX_UTIL_H
#define UNIX_UTIL_H

struct Directory;

typedef std::function<bool(const QByteArray&, const QString&, const QString&, const QString&, const QString&, const QString&, qint64, qint64, const QString&)> LFSInserter;

namespace util
{
    void processFile(const Directory& d, const QFileInfo& fileInfo, LFSInserter inserter);
};

#endif // UNIX_UTIL_H
