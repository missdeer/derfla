#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

#include <QString>

class AutoUpdater
{
public:
    static AutoUpdater *createAutoUpdate();

    virtual ~AutoUpdater();

    virtual void checkForUpdates() = 0;
};

#endif
