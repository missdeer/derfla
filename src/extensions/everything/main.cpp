#include "stdafx.h"

#include <QApplication>
#include <QIcon>

#include "util.h"
#if defined(Q_OS_WIN)
#    include "everythingwrapper.h"
#elif defined(Q_OS_MAC)
#    include "mdfindwrapper.h"
#endif

static const int maxCount = 25;

bool handleFile(const QString &pattern, bool regexpEnabled)
{
    QTextStream stdoutTs(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stdoutTs.setCodec("UTF-8");
#else
    stdoutTs.setEncoding(QStringConverter::Utf8);
#endif
    QStringList res;
    if (!QuickGetFilesByFileName(
            regexpEnabled, pattern, res, [](bool isDir) { return !isDir; }, maxCount))
    {
        stdoutTs << "searching " << pattern << "by everything failed.";
        return false;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson("[]");
    Q_ASSERT(jsonDoc.isArray());
    QJsonArray arr = jsonDoc.array();
    for (const auto &filePath : qAsConst(res))
    {
        QFileInfo fileInfo(filePath);

        QVariantMap varMap;
        varMap.insert("title", fileInfo.fileName());
        varMap.insert("description", QDir::toNativeSeparators(fileInfo.absolutePath()));
        varMap.insert("target", filePath);
        varMap.insert("arguments", "");
        varMap.insert("workingDir", "");
        varMap.insert("actionType", "revealFile");
        QByteArray bytes = util::extractPNGIconFromFile(fileInfo);
        varMap.insert("iconData", QString(bytes.toBase64()));
        arr.append(QJsonObject::fromVariantMap(varMap));
    }
    jsonDoc.setArray(arr);
    stdoutTs << QString(jsonDoc.toJson(QJsonDocument::Compact));
    return true;
}

bool handleDir(const QString &pattern, bool regexpEnabled)
{
    QTextStream stdoutTs(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stdoutTs.setCodec("UTF-8");
#else
    stdoutTs.setEncoding(QStringConverter::Utf8);
#endif
    QStringList res;
    if (!QuickGetFilesByFileName(
            regexpEnabled, pattern, res, [](bool isDir) { return isDir; }, maxCount))
    {
        stdoutTs << "searching " << pattern << "by everything failed.";
        return false;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson("[]");
    Q_ASSERT(jsonDoc.isArray());
    QJsonArray arr = jsonDoc.array();
    for (const auto &filePath : qAsConst(res))
    {
        QFileInfo fileInfo(filePath);

        QVariantMap varMap;
        varMap.insert("title", fileInfo.fileName());
        varMap.insert("description", QDir::toNativeSeparators(fileInfo.absolutePath()));
        varMap.insert("target", filePath);
        varMap.insert("arguments", "");
        varMap.insert("workingDir", "");
        varMap.insert("actionType", "shellExecute");
        varMap.insert("iconPath", QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("folder.png"));
        arr.append(QJsonObject::fromVariantMap(varMap));
    }
    jsonDoc.setArray(arr);
    stdoutTs << QString(jsonDoc.toJson(QJsonDocument::Compact));
    return true;
}

bool handleVSOpen(const QString &pattern, bool regexpEnabled)
{
    QTextStream stdoutTs(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stdoutTs.setCodec("UTF-8");
#else
    stdoutTs.setEncoding(QStringConverter::Utf8);
#endif
    QStringList res;
    if (!QuickGetFilesByFileName(
            regexpEnabled, pattern, res, [](bool isDir) { return !isDir; }, maxCount))
    {
        stdoutTs << "searching " << pattern << "by everything failed.";
        return false;
    }

    QFile scriptPath(":/open-in-vs.vbs");
    if (!scriptPath.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QByteArray script = scriptPath.readAll();
    scriptPath.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson("[]");
    Q_ASSERT(jsonDoc.isArray());
    QJsonArray arr = jsonDoc.array();
    for (const auto &filePath : qAsConst(res))
    {
        QFileInfo fileInfo(filePath);

        QStringList args {QDir::toNativeSeparators(fileInfo.absoluteFilePath()), "1", "1"};
        QVariantMap varMap;
        varMap.insert("title", fileInfo.fileName());
        varMap.insert("description", QDir::toNativeSeparators(fileInfo.absolutePath()));
        varMap.insert("target", QString(script));
        varMap.insert("arguments", args.join(QChar(' ')));
        varMap.insert("workingDir", QDir::toNativeSeparators(fileInfo.absolutePath()));
        varMap.insert("actionType", "script");
        varMap.insert("scriptExecutor", "cscript");
        QByteArray bytes = util::extractPNGIconFromFile(fileInfo);
        varMap.insert("iconData", QString(bytes.toBase64()));
        arr.append(QJsonObject::fromVariantMap(varMap));
    }
    jsonDoc.setArray(arr);
    stdoutTs << QString(jsonDoc.toJson(QJsonDocument::Compact));
    return true;
}

bool handleShellOpen(const QString &pattern, bool regexpEnabled, bool executableOnly)
{
    QTextStream stdoutTs(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stdoutTs.setCodec("UTF-8");
#else
    stdoutTs.setEncoding(QStringConverter::Utf8);
#endif
    QStringList res;
    if (!QuickGetFilesByFileName(
            regexpEnabled, pattern, res, [](bool) { return true; }, maxCount))
    {
        stdoutTs << "searching " << pattern << "by everything failed.";
        return false;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson("[]");
    Q_ASSERT(jsonDoc.isArray());
    QJsonArray arr = jsonDoc.array();
    for (const auto &filePath : qAsConst(res))
    {
        QFileInfo fileInfo(filePath);

        QVariantMap varMap;
        varMap.insert("title", fileInfo.fileName());
        varMap.insert("description", QDir::toNativeSeparators(fileInfo.absolutePath()));
        varMap.insert("target", filePath);
        varMap.insert("arguments", "");
        varMap.insert("workingDir", QDir::toNativeSeparators(fileInfo.absolutePath()));
        varMap.insert("actionType", "shellExecute");
        if (fileInfo.isDir())
        {
            varMap.insert("iconPath", QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("folder.png"));
        }
        else
        {
            QByteArray bytes = util::extractPNGIconFromFile(fileInfo);
            varMap.insert("iconData", QString(bytes.toBase64()));
        }
        arr.append(QJsonObject::fromVariantMap(varMap));
    }
    jsonDoc.setArray(arr);
    stdoutTs << QString(jsonDoc.toJson(QJsonDocument::Compact));
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
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("Everything");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationDomain("ismisv.com");
    QCoreApplication::setOrganizationName("Derfla");

    auto        uiLanguages = QLocale().uiLanguages();
    auto       &locale      = uiLanguages[0];
    QTranslator translator;
    QTranslator qtTranslator;

    // main application and dynamic linked library locale
#if defined(Q_OS_MAC)
    QString rootDirPath   = QCoreApplication::applicationDirPath() + "/../../Resources/translations";
    QString localeDirPath = QCoreApplication::applicationDirPath() + "/translations";
#else
    QString rootDirPath   = QCoreApplication::applicationDirPath() + "/../../translations";
    QString localeDirPath = QCoreApplication::applicationDirPath() + "/translations";
#endif

    if (!translator.load("everything_" + locale, localeDirPath))
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

    auto args = QCoreApplication::arguments();
    if (argc == 3 || argc == 4)
    {
        QTextStream stdoutTs(stdout);
#if defined(Q_OS_WIN)
        if (!isEverythingRunning())
        {
            stdoutTs << "everything util is not running.";
            return 1;
        }
#endif
        QString pattern   = argc == 3 ? args[2] : args[3];
        if (pattern.size() < 2)
        {
            stdoutTs << "[]";
            return 0;
        }

        if (argc == 4)
        {
            QString options(args[2]);
#if defined(Q_OS_WIN)
            Everything_SetMatchWholeWord(options.contains(QChar('w'), Qt::CaseInsensitive));
            Everything_SetMatchCase(options.contains(QChar('c'), Qt::CaseInsensitive));
            Everything_SetRegex(options.contains(QChar('r'), Qt::CaseInsensitive));
#endif
        }

        QMap<QString, std::function<bool(const QString &)>> cmdHandlerMap = {
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
        QString cmd(args[1]);
        auto    iter = cmdHandlerMap.find(cmd);
        if (cmdHandlerMap.end() == iter)
        {
            stdoutTs << "unsupported query action";
            return 3;
        }
        auto cmdHandler = cmdHandlerMap[cmd];
        if (!cmdHandler(pattern))
        {
            stdoutTs << "handler failed";
            return 4;
        }
    }

    return 0;
}
