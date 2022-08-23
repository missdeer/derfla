#ifndef MDFINDWRAPPER_H
#define MDFINDWRAPPER_H

bool QuickGetFilesByFileName(const QString &fileName, QStringList &results, std::function<bool(bool)> checker, const int count);

#endif // MDFINDWRAPPER_H
