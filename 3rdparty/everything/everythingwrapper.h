#ifndef EVERYTHINGWRAPPER_H
#define EVERYTHINGWRAPPER_H

#include <functional>

#include <QString>

bool    isEverythingRunning();
void    launchEverything(const QString &everythingFilePath);
QString GetEverythingPath();
bool QuickGetFilesByFileName(bool regexpEnabled, const QString &pattern, QStringList &results, const std::function<bool(bool)> &pfChecker, int count);

#endif // EVERYTHINGWRAPPER_H
