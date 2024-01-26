#include "stdafx.h"

#include <QIcon>

#include "heweather.h"
#include "qtsingleapplication.h"
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
    SharedTools::QtSingleApplication app("Weather", argc, argv);

    SharedTools::QtSingleApplication::setApplicationName("Weather");
    SharedTools::QtSingleApplication::setApplicationVersion("1.0");
    SharedTools::QtSingleApplication::setOrganizationDomain("ismisv.com");
    SharedTools::QtSingleApplication::setOrganizationName("Derfla");

    if (argc != 3)
    {
        QTextStream ts(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        ts.setCodec("UTF-8");
#else
        ts.setEncoding(QStringConverter::Utf8);
#endif

        ts << "invalid arguments";
        return 1;
    }

    auto args = QCoreApplication::arguments();
    QString cmd(args[1]);
    if (cmd != "w" && cmd != "weather")
    {
        QTextStream ts(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        ts.setCodec("UTF-8");
#else
        ts.setEncoding(QStringConverter::Utf8);
#endif

        ts << "invalid arguments";
        return 2;
    }

    auto        uiLanguages = QLocale().uiLanguages();
    auto       &locale      = uiLanguages[0];
    QTranslator translator;
    QTranslator qtTranslator;

    // main application and dynamic linked library locale
#if defined(Q_OS_MAC)
    QString rootDirPath   = QCoreApplication::applicationDirPath() + "/../../Resources/translations";
    QString localeDirPath = QCoreApplication::applicationDirPath() + "/translations";
#else
    QString rootDirPath = QCoreApplication::applicationDirPath() + "/../../translations";
    QString localeDirPath = QCoreApplication::applicationDirPath() + "/translations";
#endif

    if (!translator.load("weather_" + locale, localeDirPath))
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " failed";
    }
    else
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " success";
        if (!SharedTools::QtSingleApplication::installTranslator(&translator))
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
        if (!SharedTools::QtSingleApplication::installTranslator(&qtTranslator))
        {
            qDebug() << "installing qt translator failed ";
        }
    }

    QString location(args[2]);
    
    Heweather he;
    he.forecast(location);
    return QCoreApplication::exec();
}
