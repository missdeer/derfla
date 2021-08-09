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
    a.setOrganizationDomain("ismisv.com");
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

    auto        uiLanguages = QLocale().uiLanguages();
    auto &      locale      = uiLanguages[0];
    QTranslator translator;
    QTranslator qtTranslator;

    // main application and dynamic linked library locale
#if defined(Q_OS_MAC)
    QString rootDirPath = QApplication::applicationDirPath() + "/../../Resources/translations";
    QString localeDirPath = QApplication::applicationDirPath() + "/translations";
#else
    QString rootDirPath = QApplication::applicationDirPath() + "/../../translations";
    QString localeDirPath = QApplication::applicationDirPath() + "/translations";
#endif

    if (!translator.load("weather_" + locale, localeDirPath))
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " failed";
    }
    else
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " success";
        if (!a.installTranslator(&translator))
        {
            qDebug() << "installing translator failed ";
        }
    }

    // qt locale
    if (!qtTranslator.load("qt_" + locale, rootDirPath))
    {
        qDebug() << "loading " << locale << " from " << rootDirPath << " failed";
    }
    else
    {
        qDebug() << "loading " << locale << " from " << rootDirPath << " success";
        if (!a.installTranslator(&qtTranslator))
        {
            qDebug() << "installing qt translator failed ";
        }
    }

#if defined(Q_OS_WIN)
    int nArgs = 0;

    LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    QString location(QString::fromWCharArray( szArglist[2]));
    LocalFree(szArglist);
#else
    QString location(argv[2]);
#endif
    Heweather he;
    he.forecast(location);
    return a.exec();
}
