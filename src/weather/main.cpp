#include "stdafx.h"
#include "qtsingleapplication.h"
#include <QIcon>
#include "heweather.h"
#include "util.h"

int main(int argc, char *argv[])
{
#if !defined(Q_OS_WIN)
    // increase the number of file that can be opened.
    struct rlimit rl;
    getrlimit(RLIMIT_NOFILE, &rl);

    rl.rlim_cur = qMin(rl.rlim_cur, rl.rlim_max);
    setrlimit(RLIMIT_NOFILE, &rl);
#endif
    SharedTools::QtSingleApplication a("Weather", argc, argv);

    a.setApplicationName("Weather");
    a.setApplicationVersion("1.0");
    a.setOrganizationDomain("dfordsoft.com");
    a.setOrganizationName("Derfla");

    if (argc != 3)
    {
        QTextStream ts(stdout);
        ts.setCodec("UTF-8");

        ts << "invalid arguments";
        return 1;
    }

    QString cmd(argv[1]);
    if (cmd != "w" && cmd != "weather")
    {
        QTextStream ts(stdout);
        ts.setCodec("UTF-8");

        ts << "invalid arguments";
        return 2;
    }

    QString location(argv[2]);
    {
        Heweather he;
        he.forecast(location);
        return a.exec();
    }
    return -1;
}
