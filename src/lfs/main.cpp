#include "stdafx.h"
#include "qtsingleapplication.h"
#include <boost/scope_exit.hpp>
#include "dbrw.h"

int main(int argc, char *argv[])
{
#if !defined(Q_OS_WIN)
    // increase the number of file that can be opened.
    struct rlimit rl;
    getrlimit(RLIMIT_NOFILE, &rl);

    rl.rlim_cur = qMin(rl.rlim_cur, rl.rlim_max);
    setrlimit(RLIMIT_NOFILE, &rl);
#endif
    SharedTools::QtSingleApplication a("LFS", argc, argv);

    QCoreApplication::setApplicationName("LFS");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationDomain("dfordsoft.com");
    QCoreApplication::setOrganizationName("Derfla");

    if (a.isRunning())
    {
        return 0;
    }

#if defined(Q_OS_WIN)
    CoInitialize(NULL);
    BOOST_SCOPE_EXIT(void) {
        CoUninitialize();
    } BOOST_SCOPE_EXIT_END
#endif

    DBRW::instance();
    BOOST_SCOPE_EXIT(void) {
        DBRW::destroy();
    } BOOST_SCOPE_EXIT_END

    return a.exec();
}
