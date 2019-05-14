#include "stdafx.h"
#include "derflaapp.h"
#include "derflawidget.h"
#include "qtsingleapplication.h"

#if defined(Q_OS_WIN)
class AppMutex {
public:
    AppMutex() {
#if defined(_WIN64)
        m_hMutex = ::CreateMutexW(nullptr, FALSE, L"Derfla-x86_64" );
#else
        m_hMutex = ::CreateMutexW(nullptr, FALSE, L"Derfla-x86" );
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
    QCoreApplication::setOrganizationDomain("minidump.info");
    QCoreApplication::setOrganizationName("Derfla");

    if (a.isRunning())
    {
        return 0;
    }

    QDate d =  QLocale(QLocale::C).toDate(QString(__DATE__).simplified(), QLatin1String("MMM d yyyy"));
    if (d.daysTo(QDate::currentDate()) > 60)
    {
        QMessageBox::critical(nullptr, QObject::tr("Expired"), QObject::tr("This application has been expired, please visit https://minidump.info/derfla/ for a new build."), QMessageBox::Ok );
        return 1;
    }

#if defined(Q_OS_WIN)
    auto pathEnv = QDir::toNativeSeparators(QApplication::applicationDirPath()) + ";" + qgetenv("PATH");
    qDebug() << pathEnv;
    qputenv("PATH", pathEnv.toUtf8()); // so that extensions can use Derfla main executable's Qt binaries
    qputenv("QT_PLUGIN_PATH", QDir::toNativeSeparators(QApplication::applicationDirPath()).toUtf8());
#elif defined(Q_OS_MAC)
    QDir dir(a.applicationDirPath());
    dir.cdUp();
    dir.cd("Libs");
    auto pathEnv = dir.absolutePath() + ":" + qgetenv("DYLD_LIBRARY_PATH");
    qputenv("DYLD_LIBRARY_PATH", pathEnv.toUtf8());
    qputenv("QT_MAC_DISABLE_FOREGROUND_APPLICATION_TRANSFORM", "1");
#else
    auto pathEnv = dir.absolutePath() + ":" + qgetenv("LD_LIBRARY_PATH");
    qputenv("LD_LIBRARY_PATH", pathEnv.toUtf8());
#endif

    QString locale = QLocale::system().name();
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

    if (!translator.load(QString("derfla_%1.qm").arg(QLocale::system().name()), localeDirPath))
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
    if (!qtTranslator.load(QString("qt_%1.qm").arg(QLocale::system().name()), localeDirPath))
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
    app.createDerflaWidget();
    app.showDerflaWidget();
    
    return a.exec();
}
