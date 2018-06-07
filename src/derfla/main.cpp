#include "stdafx.h"
#include "derflawidget.h"
#include "qtsingleapplication.h"

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
    if (d.daysTo(QDate::currentDate()) > 30)
    {
        QMessageBox::critical(NULL, QObject::tr("Expired"), QObject::tr("This application has been expired, please visit http://www.getderfla.com for a new build."), QMessageBox::Ok );
        return 1;
    }

    auto pathEnv = qgetenv("PATH");
#if defined(Q_OS_WIN)
    pathEnv.append(";" % QDir::toNativeSeparators(a.applicationDirPath()));
#else
    pathEnv.append(":" % a.applicationDirPath());
#endif
    qputenv("QT_PLUGIN_PATH", QDir::toNativeSeparators(a.applicationDirPath()).toUtf8());
    qputenv("PATH", pathEnv); // so that extensions can use Derfla main executable's Qt binaries

    DerflaWidget w;
    w.show();

    QScreen* scr = a.primaryScreen();
    QSize sz = scr->availableSize();
    w.move(sz.width()/4, sz.height()/5);

    return a.exec();
}
