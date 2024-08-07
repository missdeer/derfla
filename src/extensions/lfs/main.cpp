#include "stdafx.h"

#include <iostream>

#include <QTextStream>

#include "dbrw.h"
#include "localfsscanner.h"
#include "localserver.h"
#include "localsocket.h"
#include "qtsingleapplication.h"
#include "scopedguard.h"

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
    ScopedGuard   cu([]() { CoUninitialize(); });
    const QString lfsLocalPipe(R"(\\.\pipe\lfsLocalPipe)");
#else
    const QString lfsLocalPipe(R"(/tmp/lfsLocalPipe)");
#endif

    if (argc == 2)
    {
        auto args = QCoreApplication::arguments();
        if (args[1].compare("/exit", Qt::CaseInsensitive) == 0 && app.isRunning())
        {
            app.sendMessage("/exit");
            return 0;
        }
        QTextStream stream(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        stream.setCodec("UTF-8");
#else
        stream.setEncoding(QStringConverter::Utf8);
#endif
        QTimer::singleShot(1500, [] { QCoreApplication::quit(); });
        LocalSocket localSocket(stream, nullptr);
        localSocket.connectToServer(lfsLocalPipe);
        localSocket.write(args[1].toUtf8());

        return QCoreApplication::exec();
    }

    if (app.isRunning())
    {
        return 0;
    }

    DBRW dbrw;

    LocalServer localServer(dbrw);
#if !defined(Q_OS_WIN)
    if (QFile::exists(lfsLocalPipe))
    {
        QFile::remove(lfsLocalPipe);
    }
#endif
    if (localServer.listen(lfsLocalPipe))
    {
        qDebug() << "listen on" << lfsLocalPipe;
    }
    else
    {
        qCritical() << "listening on pipe " << lfsLocalPipe << "failed";
    }

    LocalFSScanner scanner(dbrw);
    scanner.start();

    QObject::connect(&app, &SharedTools::QtSingleApplication::messageReceived, [&](const QString &message, QObject *) {
        if (message.compare("/exit", Qt::CaseInsensitive) == 0)
        {
            scanner.stop();
            QCoreApplication::quit();
        }
    });
    return QCoreApplication::exec();
}
