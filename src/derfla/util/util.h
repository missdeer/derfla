#ifndef UTIL_H
#define UTIL_H


namespace util
{
    QByteArray extractPNGFromIcon(const QString& filePath);
    QByteArray extractPNGIconFromFile(const QFileInfo& fi);
    QStringList &getEnvPaths();
    QStringList &getLocaleNames();
};

#endif // UTIL_H
