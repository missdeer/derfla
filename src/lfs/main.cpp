#include "stdafx.h"

#include <iostream>

#include <QTextStream>

#include "qtsingleapplication.h"
#include "scopedguard.h"
#if defined(Q_OS_WIN)
#    include "win_util.h"
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
    a.setOrganizationDomain("ismisv.com");
    a.setOrganizationName("Derfla");

#if defined(Q_OS_WIN)
    CoInitialize(NULL);
    ScopedGuard cu([]() { CoUninitialize(); });
#endif

    if (argc == 2)
    {
        if (QString(argv[1]).compare("/exit", Qt::CaseInsensitive) == 0 && a.isRunning())
        {
            a.sendMessage("/exit");
            return 0;
        }
        DBRW dbrw(true);
#if defined(Q_OS_WIN)
        int nArgs = 0;

        LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
        QString res       = dbrw.search(QString::fromWCharArray(szArglist[1]));
        LocalFree(szArglist);
#else
        QString res = dbrw.search(QString(argv[1]));
#endif
        QTextStream ts(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        ts.setCodec("UTF-8");
#else
        ts.setEncoding(QStringConverter::Utf8);
#endif
        ts << res;
        return 0;
    }

    if (a.isRunning())
    {
        return 0;
    }

    DBRW           dbrw(false);
    LocalFSScanner scanner(dbrw);
    scanner.start();

    QObject::connect(&a, &SharedTools::QtSingleApplication::messageReceived, [&](const QString &message, QObject *) {
        if (message.compare("/exit", Qt::CaseInsensitive) == 0)
        {
            scanner.stop();
            QCoreApplication::quit();
        }
    });

    return QCoreApplication::exec();
}
