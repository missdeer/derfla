#include "stdafx.h"
#include "winsparkle.h"
#include "winsparkleautoupdater.h"

WinSparkleAutoUpdater::WinSparkleAutoUpdater(const QString &url)
{
    // Setup updates feed. This must be done before win_sparkle_init(), but
    // could be also, often more conveniently, done using a VERSIONINFO Windows
    // resource. See the "psdk" example and its .rc file for an example of that
    // (these calls wouldn't be needed then).
    win_sparkle_set_appcast_url(url.toStdString().c_str());
    win_sparkle_set_app_details(L"umlgen.com", L"UMLGen", L"1.0");

    // Set DSA public key used to verify update's signature.
    // This is na example how to provide it from external source (i.e. from Qt
    // resource). See the "psdk" example and its .rc file for an example how to
    // provide the key using Windows resource.
    win_sparkle_set_dsa_pub_pem(reinterpret_cast<const char *>(QResource(":/rc/dsa_pub.pem").data()));

    // Initialize the updater and possibly show some UI
    win_sparkle_init();
}

WinSparkleAutoUpdater::~WinSparkleAutoUpdater()
{
    win_sparkle_cleanup();
}

void WinSparkleAutoUpdater::checkForUpdates()
{
    win_sparkle_check_update_with_ui();
}
