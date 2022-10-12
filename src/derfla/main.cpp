#include "stdafx.h"

#include "derflaapp.h"
#include "qtsingleapplication.h"

#if defined(Q_OS_WIN)
class AppMutex
{
public:
    AppMutex()
    {
#    if defined(_WIN64)
        m_hMutex = ::CreateMutexW(nullptr, FALSE, L"Derfla-x86_64");
#    else
        m_hMutex = ::CreateMutexW(nullptr, FALSE, L"Derfla-x86");
#    endif
    }
    ~AppMutex()
    {
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

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QCoreApplication::setApplicationName("Derfla");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationDomain("ismisv.com");
    QCoreApplication::setOrganizationName("Derfla");

    SharedTools::QtSingleApplication a("Derfla", argc, argv);
    if (a.isRunning())
    {
        return 0;
    }

    auto        uiLanguages = QLocale().uiLanguages();
    QString     locale      = uiLanguages[0];
    QTranslator translator;
    QTranslator qtTranslator;

    // main application and dynamic linked library locale
#if defined(Q_OS_MAC)
    QString localeDirPath = QApplication::applicationDirPath() + "/../Resources/translations";
#else
    QString localeDirPath = QApplication::applicationDirPath() + "/translations";
    if (!QDir(localeDirPath).exists())
    {
        localeDirPath = QApplication::applicationDirPath() + "/../translations";
    }
#endif

    if (!translator.load(QString("derfla_%1.qm").arg(locale), localeDirPath))
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " failed";
    }
    else
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " success";
        if (!QApplication::installTranslator(&translator))
        {
            qDebug() << "installing translator failed ";
        }
    }

    // qt locale
    if (!qtTranslator.load(QString("qt_%1.qm").arg(locale), localeDirPath))
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " failed";
    }
    else
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " success";
        if (!QApplication::installTranslator(&qtTranslator))
        {
            qDebug() << "installing qt translator failed ";
        }
    }

    DerflaApp app;
    derflaApp = &app;
    derflaApp->createWidget();
    derflaApp->show();

    return a.exec();
}
