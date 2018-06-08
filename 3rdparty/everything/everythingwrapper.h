#ifndef EVERYTHINGWRAPPER_H
#define EVERYTHINGWRAPPER_H

bool isEverythingRunning();
void launchEverything(const QString& everythingFilePath);
QString GetEverythingPath();
bool QuickGetFilesByFileName(const QString& fileName, QStringList& results, std::function<bool(bool)> checker,const int count);

#endif // EVERYTHINGWRAPPER_H
