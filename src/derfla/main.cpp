#include "stdafx.h"
#include "derflawidget.h"
#include "qtsingleapplication.h"

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

    if (a.isRunning())
    {
        return 0;
    }

    QDate d =  QLocale(QLocale::C).toDate(QString(__DATE__).simplified(), QLatin1String("MMM d yyyy"));
    if (d.daysTo(QDate::currentDate()) > 365)
    {
        QMessageBox::critical(NULL, "Expired", "This application has been expired, please contact fyang3@cisco.com for a new build.", QMessageBox::Ok );
        return 1;
    }

    DerflaWidget w;
    w.show();

    QScreen* scr = a.primaryScreen();
    QSize sz = scr->availableSize();
    w.move(sz.width()/4, sz.height()/5);

    return a.exec();
}
