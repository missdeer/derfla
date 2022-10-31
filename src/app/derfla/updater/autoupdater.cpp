#include "stdafx.h"
#if defined(Q_OS_WIN)
#    include "winsparkleautoupdater.h"
#elif defined(Q_OS_MAC)
#    include "CocoaInitializer.h"
#    include "SparkleAutoUpdater.h"
#endif
#include "autoupdater.h"

AutoUpdater *AutoUpdater::createAutoUpdate()
{
#if defined(Q_OS_WIN)
    return new WinSparkleAutoUpdater("https://derfla.dfordsoft.com/dl/w/appcast.xml");
#elif defined(Q_OS_MAC)
    CocoaInitializer initializer;
    return new SparkleAutoUpdater("https://derfla.dfordsoft.com/dl/m/appcast.xml");
#endif
    return nullptr;
}

AutoUpdater::~AutoUpdater() {}
