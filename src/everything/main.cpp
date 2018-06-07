#include "stdafx.h"
#include "qtsingleapplication.h"
#include "everythingwrapper.h"

int main(int argc, char *argv[])
{
#if !defined(Q_OS_WIN)
    // increase the number of file that can be opened.
    struct rlimit rl;
    getrlimit(RLIMIT_NOFILE, &rl);

    rl.rlim_cur = qMin(rl.rlim_cur, rl.rlim_max);
    setrlimit(RLIMIT_NOFILE, &rl);
#endif
    SharedTools::QtSingleApplication a("Everything", argc, argv);

    a.setApplicationName("Everything");
    a.setApplicationVersion("1.0");
    a.setOrganizationDomain("dfordsoft.com");
    a.setOrganizationName("Derfla");

    if (argc == 2)
    {
        QTextStream ts( stdout );
        if (!isEverythingRunning())
        {
            ts << "everything util is not running.";
            return 1;
        }
        QStringList res;
        if (!QuickGetFilesByFileName(QString(argv[1]), res))
        {
            ts << "searching " << argv[1] << "by everything failed.";
            return 2;
        }

    }

    return 0;
}
