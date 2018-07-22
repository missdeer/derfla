#include "stdafx.h"
#include "qtsingleapplication.h"
#include <boost/scope_exit.hpp>
#include <QTextStream>
#include <iostream>
#if defined(Q_OS_WIN)
#include "win_util.h"
#endif
#include "dbrw.h"
#include "localfsitem.h"
#include "localfsscanner.h"

using namespace std;


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

    a.setApplicationName("LFS");
    a.setApplicationVersion("1.0");
    a.setOrganizationDomain("dfordsoft.com");
    a.setOrganizationName("Derfla");

#if defined(Q_OS_WIN)
    CoInitialize(NULL);
    BOOST_SCOPE_EXIT(void) {
        CoUninitialize();
    } BOOST_SCOPE_EXIT_END
#endif

    DBRW dbrw;
    if (argc == 2)
    {
        if (QString(argv[1]).compare("/exit", Qt::CaseInsensitive) == 0 && a.isRunning())
        {
            a.sendMessage("/exit");
            return 0;
        }
        QString res = dbrw.search(QString(argv[1]));
        QTextStream ts( stdout );
        ts.setCodec("UTF-8");
        ts << res;
        return 0;
    }

    if (a.isRunning())
        return 0;

    LocalFSScanner scanner(dbrw);
    scanner.start();

    QObject::connect(&a, &SharedTools::QtSingleApplication::messageReceived, [&](const QString &message, QObject *){
        if (message.compare("/exit", Qt::CaseInsensitive) == 0)
        {
            scanner.stop();
            a.quit();
        }
    });

    return a.exec();
}
