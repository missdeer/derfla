#include "stdafx.h"

#include <iostream>

#include <QTextStream>

#include "dbrw.h"
#include "localfsscanner.h"
#include "localserver.h"
#include "localsocket.h"
#include "qtsingleapplication.h"
#include "scopedguard.h"

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

    SharedTools::QtSingleApplication app("LFS", argc, argv);

    QCoreApplication::setApplicationName("LFS");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationDomain("ismisv.com");
    QCoreApplication::setOrganizationName("Derfla");

#if defined(Q_OS_WIN)
    CoInitialize(NULL);
    ScopedGuard cu([]() { CoUninitialize(); });
#endif

    const QString lfsLocalPipe(R"(\\.\pipe\lfsLocalPipe)");
    if (argc == 2)
    {
        if (QString(argv[1]).compare("/exit", Qt::CaseInsensitive) == 0 && app.isRunning())
        {
            app.sendMessage("/exit");
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

    if (app.isRunning())
    {
        return 0;
    }

    DBRW           dbrw(false);
    LocalFSScanner scanner(dbrw);
    scanner.start();

    QObject::connect(&app, &SharedTools::QtSingleApplication::messageReceived, [&](const QString &message, QObject *) {
        if (message.compare("/exit", Qt::CaseInsensitive) == 0)
        {
            scanner.stop();
            QCoreApplication::quit();
        }
    });

    LocalServer localServer;
    localServer.listen(lfsLocalPipe);

    return QCoreApplication::exec();
}
