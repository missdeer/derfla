#include "stdafx.h"

#include <QIcon>

#include "qtsingleapplication.h"
#include "util.h"
#include "youdao.h"

int main(int argc, char *argv[])
{
#if !defined(Q_OS_WIN)
    // increase the number of file that can be opened.
    struct rlimit rl;
    getrlimit(RLIMIT_NOFILE, &rl);

    rl.rlim_cur = qMin(rl.rlim_cur, rl.rlim_max);
    setrlimit(RLIMIT_NOFILE, &rl);
#endif
    SharedTools::QtSingleApplication app("WebDictionary", argc, argv);

    QCoreApplication::setApplicationName("WebDictionary");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationDomain("ismisv.com");
    QCoreApplication::setOrganizationName("Derfla");

    if (argc < 3)
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

    if (!translator.load("webdict_" + locale, localeDirPath))
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " failed";
    }
    else
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " success";
        if (!QCoreApplication::installTranslator(&translator))
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
        if (!QCoreApplication::installTranslator(&qtTranslator))
        {
            qDebug() << "installing qt translator failed ";
        }
    }

    QString     cmd(argv[1]);
    QStringList input;

#if defined(Q_OS_WIN)
    int nArgs = 0;

    LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    for (int i = 2; i < argc; i++)
    {
        input.append(QString::fromWCharArray(szArglist[i]));
    }
    LocalFree(szArglist);
#else
    for (int i = 2; i < argc; i++)
    {
        input.append(QString(argv[i]));
    }
#endif
    if (cmd == "dict" || cmd == "yd" || cmd == "youdao")
    {
        Youdao yd;
        yd.query(input.join(' '));
        return QCoreApplication::exec();
    }

    return -1;
}
