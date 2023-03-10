#include "stdafx.h"

#include <QIcon>

#include "qtsingleapplication.h"
#include "util.h"
#if defined(Q_OS_WIN)
#    include "everythingwrapper.h"
#elif defined(Q_OS_MAC)
#    include "mdfindwrapper.h"
#endif

static const int maxCount = 25;

bool handleFile(const QString &pattern, bool regexpEnabled)
{
    QTextStream ts(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    ts.setCodec("UTF-8");
#else
    ts.setEncoding(QStringConverter::Utf8);
#endif
    QStringList res;
    if (!QuickGetFilesByFileName(
            regexpEnabled, pattern, res, [](bool isDir) { return !isDir; }, maxCount))
    {
        ts << "searching " << pattern << "by everything failed.";
        return false;
    }

    QJsonDocument d = QJsonDocument::fromJson("[]");
    Q_ASSERT(d.isArray());
    QJsonArray arr = d.array();
    for (const auto &f : qAsConst(res))
    {
        QFileInfo fi(f);

        QVariantMap m;
        m.insert("title", fi.fileName());
        m.insert("description", QDir::toNativeSeparators(fi.absolutePath()));
        m.insert("target", QDir::toNativeSeparators(fi.absoluteFilePath()));
        m.insert("arguments", "");
        m.insert("workingDir", "");
        m.insert("actionType", "revealFile");
        QByteArray bytes = util::extractPNGIconFromFile(fi);
        m.insert("iconData", QString(bytes.toBase64()));
        arr.append(QJsonObject::fromVariantMap(m));
    }
    d.setArray(arr);
    ts << QString(d.toJson(QJsonDocument::Compact));
    return true;
}

bool handleDir(const QString &pattern, bool regexpEnabled)
{
    QTextStream ts(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    ts.setCodec("UTF-8");
#else
    ts.setEncoding(QStringConverter::Utf8);
#endif
    QStringList res;
    if (!QuickGetFilesByFileName(
            regexpEnabled, pattern, res, [](bool isDir) { return isDir; }, maxCount))
    {
        ts << "searching " << pattern << "by everything failed.";
        return false;
    }

    QJsonDocument d = QJsonDocument::fromJson("[]");
    Q_ASSERT(d.isArray());
    QJsonArray arr = d.array();
    for (const auto &f : qAsConst(res))
    {
        QFileInfo fi(f);

        QVariantMap m;
        m.insert("title", fi.fileName());
        m.insert("description", QDir::toNativeSeparators(fi.absolutePath()));
        m.insert("target", QDir::toNativeSeparators(fi.absoluteFilePath()));
        m.insert("arguments", "");
        m.insert("workingDir", "");
        m.insert("actionType", "shellExecute");
        m.insert("iconPath", QString(QCoreApplication::applicationDirPath() % "/folder.png"));
        arr.append(QJsonObject::fromVariantMap(m));
    }
    d.setArray(arr);
    ts << QString(d.toJson(QJsonDocument::Compact));
    return true;
}

bool handleVSOpen(const QString &pattern, bool regexpEnabled)
{
    QTextStream ts(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    ts.setCodec("UTF-8");
#else
    ts.setEncoding(QStringConverter::Utf8);
#endif
    QStringList res;
    if (!QuickGetFilesByFileName(
            regexpEnabled, pattern, res, [](bool isDir) { return !isDir; }, maxCount))
    {
        ts << "searching " << pattern << "by everything failed.";
        return false;
    }

    QFile f(":/open-in-vs.vbs");
    if (!f.open(QIODevice::ReadOnly))
        return false;
    QByteArray c = f.readAll();
    f.close();

    QJsonDocument d = QJsonDocument::fromJson("[]");
    Q_ASSERT(d.isArray());
    QJsonArray arr = d.array();
    for (const auto &f : qAsConst(res))
    {
        QFileInfo fi(f);

        QStringList args;
        args << QDir::toNativeSeparators(fi.absoluteFilePath()) << "1"
             << "1";
        QVariantMap m;
        m.insert("title", fi.fileName());
        m.insert("description", QDir::toNativeSeparators(fi.absolutePath()));
        m.insert("target", QString(c));
        m.insert("arguments", args.join(QChar(' ')));
        m.insert("workingDir", QDir::toNativeSeparators(fi.absolutePath()));
        m.insert("actionType", "script");
        m.insert("scriptExecutor", "cscript");
        QByteArray bytes = util::extractPNGIconFromFile(fi);
        m.insert("iconData", QString(bytes.toBase64()));
        arr.append(QJsonObject::fromVariantMap(m));
    }
    d.setArray(arr);
    ts << QString(d.toJson(QJsonDocument::Compact));
    return true;
}

bool handleShellOpen(const QString &pattern, bool regexpEnabled, bool executableOnly)
{
    QTextStream ts(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    ts.setCodec("UTF-8");
#else
    ts.setEncoding(QStringConverter::Utf8);
#endif
    QStringList res;
    if (!QuickGetFilesByFileName(
            regexpEnabled, pattern, res, [](bool) { return true; }, maxCount))
    {
        ts << "searching " << pattern << "by everything failed.";
        return false;
    }

    QJsonDocument d = QJsonDocument::fromJson("[]");
    Q_ASSERT(d.isArray());
    QJsonArray arr = d.array();
    for (const auto &f : qAsConst(res))
    {
        QFileInfo fi(f);

        QVariantMap m;
        m.insert("title", fi.fileName());
        m.insert("description", QDir::toNativeSeparators(fi.absolutePath()));
        m.insert("target", QDir::toNativeSeparators(fi.absoluteFilePath()));
        m.insert("arguments", "");
        m.insert("workingDir", QDir::toNativeSeparators(fi.absolutePath()));
        m.insert("actionType", "shellExecute");
        if (fi.isDir())
        {
            m.insert("iconPath", QString(QCoreApplication::applicationDirPath() % "/folder.png"));
        }
        else
        {
            QByteArray bytes = util::extractPNGIconFromFile(fi);
            m.insert("iconData", QString(bytes.toBase64()));
        }
        arr.append(QJsonObject::fromVariantMap(m));
    }
    d.setArray(arr);
    ts << QString(d.toJson(QJsonDocument::Compact));
    return true;
}

int main(int argc, char *argv[])
{
#if !defined(Q_OS_WIN)
    // increase the number of file that can be opened.
    struct rlimit rl;
    getrlimit(RLIMIT_NOFILE, &rl);

    rl.rlim_cur = qMin(rl.rlim_cur, rl.rlim_max);
    setrlimit(RLIMIT_NOFILE, &rl);
#endif
    SharedTools::QtSingleApplication a("Everything", argc, argv);

    a.setApplicationName("Everything");
    a.setApplicationVersion("1.0");
    a.setOrganizationDomain("ismisv.com");
    a.setOrganizationName("Derfla");

    auto        uiLanguages = QLocale().uiLanguages();
    auto       &locale      = uiLanguages[0];
    QTranslator translator;
    QTranslator qtTranslator;

    // main application and dynamic linked library locale
#if defined(Q_OS_MAC)
    QString rootDirPath   = QApplication::applicationDirPath() + "/../../Resources/translations";
    QString localeDirPath = QApplication::applicationDirPath() + "/translations";
#else
    QString rootDirPath   = QApplication::applicationDirPath() + "/../../translations";
    QString localeDirPath = QApplication::applicationDirPath() + "/translations";
#endif

    if (!translator.load("everything_" + locale, localeDirPath))
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

    if (argc == 3 || argc == 4)
    {
        QTextStream ts(stdout);
#if defined(Q_OS_WIN)
        if (!isEverythingRunning())
        {
            ts << "everything util is not running.";
            return 1;
        }

        int nArgs = 0;

        LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
        QString pattern   = QString::fromWCharArray(argc == 3 ? szArglist[2] : szArglist[3]);
        LocalFree(szArglist);
#else
        QString pattern(argc == 3 ? argv[2] : argv[3]);
#endif
        if (pattern.size() < 2)
        {
            ts << "[]";
            return 0;
        }

        if (argc == 4)
        {
            QString options(argv[2]);
#if defined(Q_OS_WIN)
            Everything_SetMatchWholeWord(options.contains(QChar('w'), Qt::CaseInsensitive));
            Everything_SetMatchCase(options.contains(QChar('c'), Qt::CaseInsensitive));
            Everything_SetRegex(options.contains(QChar('r'), Qt::CaseInsensitive));
#endif
        }

        QMap<QString, std::function<bool(const QString &)>> m = {
            {"f", [](auto &&cmd) { return handleFile(std::forward<decltype(cmd)>(cmd), false); }},
            {"d", [](auto &&cmd) { return handleDir(std::forward<decltype(cmd)>(cmd), false); }},
            {"vs", [](auto &&cmd) { return handleVSOpen(std::forward<decltype(cmd)>(cmd), false); }},
            {"open", [](auto &&cmd) { return handleShellOpen(std::forward<decltype(cmd)>(cmd), false, false); }},
            {"run", [](auto &&cmd) { return handleShellOpen(std::forward<decltype(cmd)>(cmd), false, true); }},
            {"o", [](auto &&cmd) { return handleShellOpen(std::forward<decltype(cmd)>(cmd), false, false); }},
            {"r", [](auto &&cmd) { return handleShellOpen(std::forward<decltype(cmd)>(cmd), false, true); }},
            {"rf", [](auto &&cmd) { return handleFile(std::forward<decltype(cmd)>(cmd), true); }},
            {"rd", [](auto &&cmd) { return handleDir(std::forward<decltype(cmd)>(cmd), true); }},
            {"rvs", [](auto &&cmd) { return handleVSOpen(std::forward<decltype(cmd)>(cmd), true); }},
            {"ropen", [](auto &&cmd) { return handleShellOpen(std::forward<decltype(cmd)>(cmd), true, false); }},
            {"rrun", [](auto &&cmd) { return handleShellOpen(std::forward<decltype(cmd)>(cmd), true, true); }},
            {"rr", [](auto &&cmd) { return handleShellOpen(std::forward<decltype(cmd)>(cmd), true, true); }},
            {"ro", [](auto &&cmd) { return handleShellOpen(std::forward<decltype(cmd)>(cmd), true, false); }},
        };
        QString cmd(argv[1]);
        auto    it = m.find(cmd);
        if (m.end() == it)
        {
            ts << "unsupported query action";
            return 3;
        }
        auto f = m[cmd];
        if (!f(pattern))
        {
            ts << "handler failed";
            return 4;
        }
    }

    return 0;
}
