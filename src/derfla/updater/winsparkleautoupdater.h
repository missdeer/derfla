#ifndef WINSPARKLEAUTOUPDATER_H
#define WINSPARKLEAUTOUPDATER_H

#include <QString>

#include "autoupdater.h"

class WinSparkleAutoUpdater : public AutoUpdater
{
public:
    explicit WinSparkleAutoUpdater(const QString &url);
    ~WinSparkleAutoUpdater();

    void checkForUpdates();
};

#endif // WINSPARKLEAUTOUPDATER_H
