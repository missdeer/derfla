#ifndef EVERYTHINGWRAPPER_H
#define EVERYTHINGWRAPPER_H

#include <QString>
#include <functional>

bool isEverythingRunning();
void launchEverything(const QString& everythingFilePath);
QString GetEverythingPath();
bool QuickGetFilesByFileName(bool regexpEnabled, const QString &fileName, QStringList &results, std::function<bool(bool)> checker, const int count);

#endif // EVERYTHINGWRAPPER_H
