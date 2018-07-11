#include "stdafx.h"
#include "derflawidget.h"
#include "qtsingleapplication.h"

#if defined(Q_OS_WIN)
class AppMutex {
public:
    AppMutex() {
#if defined(_WIN64)
        m_hMutex = ::CreateMutexW(NULL, FALSE, L"Derfla-x86_64" );
#else
        m_hMutex = ::CreateMutexW(NULL, FALSE, L"Derfla-x86" );
#endif
    }
    ~AppMutex() {
        if (m_hMutex)
        {
            ::ReleaseMutex(m_hMutex);
        }
    }
private:
    HANDLE m_hMutex;
};

static AppMutex g_am;
#endif

int main(int argc, char *argv[])
{
#if !defined(Q_OS_WIN)
    // increase the number of file that can be opened.
    struct rlimit rl;
    getrlimit(RLIMIT_NOFILE, &rl);

    rl.rlim_cur = qMin(rl.rlim_cur, rl.rlim_max);
    setrlimit(RLIMIT_NOFILE, &rl);
#endif
    SharedTools::QtSingleApplication a("Derfla", argc, argv);

    QCoreApplication::setApplicationName("Derfla");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationDomain("dfordsoft.com");
    QCoreApplication::setOrganizationName("Derfla");

    if (a.isRunning())
    {
        return 0;
    }

    QDate d =  QLocale(QLocale::C).toDate(QString(__DATE__).simplified(), QLatin1String("MMM d yyyy"));
    if (d.daysTo(QDate::currentDate()) > 60)
    {
        QMessageBox::critical(NULL, QObject::tr("Expired"), QObject::tr("This application has been expired, please visit http://www.getderfla.com for a new build."), QMessageBox::Ok );
        return 1;
    }

#if defined(Q_OS_WIN)
    auto pathEnv = qgetenv("PATH");
    pathEnv.append(";" % QDir::toNativeSeparators(a.applicationDirPath()));
    qputenv("PATH", pathEnv); // so that extensions can use Derfla main executable's Qt binaries
    qputenv("QT_PLUGIN_PATH", QDir::toNativeSeparators(a.applicationDirPath()).toUtf8());
#elif defined(Q_OS_MAC)
    auto pathEnv = qgetenv("DYLD_LIBRARY_PATH");
    QDir dir(a.applicationDirPath());
    dir.cdUp();
    dir.cd("Libs");
    pathEnv.append(":" % dir.absolutePath());
    qputenv("DYLD_LIBRARY_PATH", pathEnv);
    qputenv("QT_MAC_DISABLE_FOREGROUND_APPLICATION_TRANSFORM", "1");
#else
    auto pathEnv = qgetenv("LD_LIBRARY_PATH");
    pathEnv.append(":" % a.applicationDirPath());
    qputenv("LD_LIBRARY_PATH", pathEnv);
#endif

    DerflaWidget w;
    w.show();

    QScreen* scr = a.primaryScreen();
    QSize sz = scr->availableSize();
    w.move(sz.width()/4, sz.height()/5);

    return a.exec();
}
