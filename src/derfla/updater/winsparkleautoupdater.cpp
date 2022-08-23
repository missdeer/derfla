#include "stdafx.h"

#include "winsparkleautoupdater.h"
#include "winsparkle.h"

WinSparkleAutoUpdater::WinSparkleAutoUpdater(const QString &url)
{
    // Setup updates feed. This must be done before win_sparkle_init(), but
    // could be also, often more conveniently, done using a VERSIONINFO Windows
    // resource. See the "psdk" example and its .rc file for an example of that
    // (these calls wouldn't be needed then).
    win_sparkle_set_appcast_url(url.toStdString().c_str());
    win_sparkle_set_app_details(L"dfordsoft.com", L"Derfla", L"1.0.0.1");

    // Set DSA public key used to verify update's signature.
    // This is na example how to provide it from external source (i.e. from Qt
    // resource). See the "psdk" example and its .rc file for an example how to
    // provide the key using Windows resource.
    win_sparkle_set_dsa_pub_pem(reinterpret_cast<const char *>(QResource(":/rc/updater-dsa-pub.pem").data()));

    win_sparkle_set_automatic_check_for_updates(1);
    win_sparkle_set_update_check_interval(24 * 3600);
    // Initialize the updater and possibly show some UI
    win_sparkle_init();
}

WinSparkleAutoUpdater::~WinSparkleAutoUpdater()
{
    win_sparkle_cleanup();
}

void WinSparkleAutoUpdater::checkForUpdates()
{
    win_sparkle_check_update_without_ui();
}
