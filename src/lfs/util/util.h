#ifndef UTIL_H
#define UTIL_H


namespace util
{
    extern qint64 timestamp;
    QByteArray extractPNGFromIcon(const QString& filePath);
    QByteArray extractPNGIconFromFile(const QFileInfo& fi);
    QStringList &getEnvPaths();
    QStringList &getLocaleNames();
};

#endif // UTIL_H
