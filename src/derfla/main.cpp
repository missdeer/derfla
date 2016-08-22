#include "stdafx.h"
#include "derflawidget.h"
#include "qtsingleapplication.h"
#include <boost/scope_exit.hpp>

int main(int argc, char *argv[])
{
#if !defined(Q_OS_WIN)
    // increase the number of file that can be opened.
    struct rlimit rl;
    getrlimit(RLIMIT_NOFILE, &rl);

    rl.rlim_cur = qMin((rlim_t)OPEN_MAX, rl.rlim_max);
    setrlimit(RLIMIT_NOFILE, &rl);
#endif
    SharedTools::QtSingleApplication a("Derfla", argc, argv);

    QCoreApplication::setApplicationName("Derfla");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationDomain("dfordsoft.com");
    QCoreApplication::setOrganizationName("DForD Software");

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

#if defined(Q_OS_WIN)
    CoInitialize(NULL);
    BOOST_SCOPE_EXIT(void) {
        CoUninitialize();
    } BOOST_SCOPE_EXIT_END
#endif

    DerflaWidget w;
    w.show();

    QScreen* scr = a.primaryScreen();
    QSize sz = scr->availableSize();
    w.move(sz.width()/4, sz.height()/5);

    return a.exec();
}
