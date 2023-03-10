#ifndef MDFINDWRAPPER_H
#define MDFINDWRAPPER_H

bool QuickGetFilesByFileName(
    bool regexpEnabled, const QString &fileName, QStringList &results, const std::function<bool(bool)> &checker, int maxCount);

#endif // MDFINDWRAPPER_H
