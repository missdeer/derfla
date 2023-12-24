#include "stdafx.h"

#include <QIcon>

#include "qtsingleapplication.h"
#include "util.h"

// title, target, arguments, icon file
using GPTActionItem = std::tuple<QString, QString, QString, QString>;

bool output(const std::vector<GPTActionItem> &items)
{
    QJsonDocument doc = QJsonDocument::fromJson("[]");
    Q_ASSERT(doc.isArray());
    QJsonArray arr = doc.array();
    for (const auto &item : items)
    {
        QVariantMap varMap;
        varMap.insert("title", std::get<0>(item));
        varMap.insert("target", std::get<1>(item));
        varMap.insert("actionType", "shellExecute");
        QFile icon(std::get<3>(item));
        if (icon.open(QIODevice::ReadOnly))
        {
            auto bytes = icon.readAll();
            icon.close();
            varMap.insert("iconData", bytes.toBase64());
        }
        arr.append(QJsonObject::fromVariantMap(varMap));
    }

    doc.setArray(arr);
    QTextStream ts(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    ts.setCodec("UTF-8");
#else
    ts.setEncoding(QStringConverter::Utf8);
#endif
    ts << QString(doc.toJson(QJsonDocument::Compact));
    return true;
}

bool setAPIKeyHandler(const QString &keyword)
{
    auto executablePath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());

    std::vector<GPTActionItem> items = {
        {QObject::tr("Set OpenAI GPT3.5 API Key"),
         executablePath,
         QStringLiteral("-gpt35key %1").arg(keyword),
         QStringLiteral(":/rc/images/openai.png")},
        {QObject::tr("Set OpenAI GPT4 API Key"),
         executablePath,
         QStringLiteral("-gpt4key %1").arg(keyword),
         QStringLiteral(":/rc/images/openai.png")},
        {QObject::tr("Set Azure OpenAI GPT3.5 API Key"),
         executablePath,
         QStringLiteral("-azure35key %1").arg(keyword),
         QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Set Azure OpenAI GPT4 API Key"),
         executablePath,
         QStringLiteral("-azure4key %1").arg(keyword),
         QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Set Gemini Pro API Key"),
         executablePath,
         QStringLiteral("-geminiprokey %1").arg(keyword),
         QStringLiteral(":/rc/images/geminipro.png")},
    };
    return output(items);
}

bool clearSetAPIKeyHandler(const QString &keyword)
{
    auto                       executablePath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    std::vector<GPTActionItem> items          = {
        {QObject::tr("Set OpenAI GPT3.5 API Key"),
                  executablePath,
                  QStringLiteral("-gpt35key %1").arg(keyword),
                  QStringLiteral(":/rc/images/openai.png")},
        {QObject::tr("Set OpenAI GPT4 API Key"),
                  executablePath,
                  QStringLiteral("-gpt4key %1").arg(keyword),
                  QStringLiteral(":/rc/images/openai.png")},
        {QObject::tr("Set Azure OpenAI GPT3.5 API Key"),
                  executablePath,
                  QStringLiteral("-azure35key %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Set Azure OpenAI GPT4 API Key"),
                  executablePath,
                  QStringLiteral("-azure4key %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Set Gemini Pro API Key"),
                  executablePath,
                  QStringLiteral("-geminiprokey %1").arg(keyword),
                  QStringLiteral(":/rc/images/geminipro.png")},
        {QObject::tr("Clear OpenAI GPT3.5 API Key"),
                  executablePath,
                  QStringLiteral("-cleargpt35key %1").arg(keyword),
                  QStringLiteral(":/rc/images/openai.png")},
        {QObject::tr("Clear OpenAI GPT4 API Key"),
                  executablePath,
                  QStringLiteral("-cleargpt4key %1").arg(keyword),
                  QStringLiteral(":/rc/images/openai.png")},
        {QObject::tr("Clear Azure OpenAI GPT3.5 API Key"),
                  executablePath,
                  QStringLiteral("-clearauzre35key %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Clear Azure OpenAI GPT4 API Key"),
                  executablePath,
                  QStringLiteral("-clearazure4key %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Clear Gemini Pro API Key"),
                  executablePath,
                  QStringLiteral("-cleargeminiprokey %1").arg(keyword),
                  QStringLiteral(":/rc/images/geminipro.png")},
    };
    return output(items);
}

bool setAPIEndpointHandler(const QString &keyword)
{
    auto                       executablePath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    std::vector<GPTActionItem> items          = {
        {QObject::tr("Set Azure OpenAI GPT3.5 API Endpoint"),
                  executablePath,
                  QStringLiteral("-azure35ep %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Set Azure OpenAI GPT4 API Endpoint"),
                  executablePath,
                  QStringLiteral("-auzre4ep %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
    };
    return output(items);
}

bool setAPIKeyEndpointHandler(const QString &keyword)
{
    auto                       executablePath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    std::vector<GPTActionItem> items          = {
        {QObject::tr("Set OpenAI GPT3.5 API Key"),
                  executablePath,
                  QStringLiteral("-gpt35key %1").arg(keyword),
                  QStringLiteral(":/rc/images/openai.png")},
        {QObject::tr("Set OpenAI GPT4 API Key"),
                  executablePath,
                  QStringLiteral("-gpt4key %1").arg(keyword),
                  QStringLiteral(":/rc/images/openai.png")},
        {QObject::tr("Set Azure OpenAI GPT3.5 API Key"),
                  executablePath,
                  QStringLiteral("-azure35key %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Set Azure OpenAI GPT4 API Key"),
                  executablePath,
                  QStringLiteral("-azure4key %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Set Gemini Pro API Key"),
                  executablePath,
                  QStringLiteral("-geminiprokey %1").arg(keyword),
                  QStringLiteral(":/rc/images/geminipro.png")},
        {QObject::tr("Set Azure OpenAI GPT3.5 API Endpoint"),
                  executablePath,
                  QStringLiteral("-azure35ep %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Set Azure OpenAI GPT4 API Endpoint"),
                  executablePath,
                  QStringLiteral("-auzre4ep %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
    };
    return output(items);
}

bool clearAPIKeyHandler(const QString &keyword)
{
    auto                       executablePath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    std::vector<GPTActionItem> items          = {
        {QObject::tr("Clear OpenAI GPT3.5 API Key"),
                  executablePath,
                  QStringLiteral("-cleargpt35key %1").arg(keyword),
                  QStringLiteral(":/rc/images/openai.png")},
        {QObject::tr("Clear OpenAI GPT4 API Key"),
                  executablePath,
                  QStringLiteral("-cleargpt4key %1").arg(keyword),
                  QStringLiteral(":/rc/images/openai.png")},
        {QObject::tr("Clear Azure OpenAI GPT3.5 API Key"),
                  executablePath,
                  QStringLiteral("-clearauzre35key %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Clear Azure OpenAI GPT4 API Key"),
                  executablePath,
                  QStringLiteral("-clearazure4key %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Clear Gemini Pro API Key"),
                  executablePath,
                  QStringLiteral("-cleargeminiprokey %1").arg(keyword),
                  QStringLiteral(":/rc/images/geminipro.png")},
    };
    return output(items);
}

bool clearAPIEndpointHandler(const QString &keyword)
{
    auto                       executablePath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    std::vector<GPTActionItem> items          = {
        {QObject::tr("Clear Azure OpenAI GPT3.5 API Endpoint"),
                  executablePath,
                  QStringLiteral("-clearazure35ep %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Clear Azure OpenAI GPT4 API Endpoint"),
                  executablePath,
                  QStringLiteral("-clearazure4ep %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
    };
    return output(items);
}

bool clearSetAPIEndpointHandler(const QString &keyword)
{
    auto                       executablePath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    std::vector<GPTActionItem> items          = {
        {QObject::tr("Set Azure OpenAI GPT3.5 API Endpoint"),
                  executablePath,
                  QStringLiteral("-azure35ep %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Set Azure OpenAI GPT4 API Endpoint"),
                  executablePath,
                  QStringLiteral("-auzre4ep %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Clear Azure OpenAI GPT3.5 API Endpoint"),
                  executablePath,
                  QStringLiteral("-clearazure35ep %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Clear Azure OpenAI GPT4 API Endpoint"),
                  executablePath,
                  QStringLiteral("-clearazure4ep %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
    };
    return output(items);
}

bool clearAPIKeyEndpointHandler(const QString &keyword)
{
    auto                       executablePath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    std::vector<GPTActionItem> items          = {
        {QObject::tr("Clear OpenAI GPT3.5 API Key"),
                  executablePath,
                  QStringLiteral("-cleargpt35key %1").arg(keyword),
                  QStringLiteral(":/rc/images/openai.png")},
        {QObject::tr("Clear OpenAI GPT4 API Key"),
                  executablePath,
                  QStringLiteral("-cleargpt4key %1").arg(keyword),
                  QStringLiteral(":/rc/images/openai.png")},
        {QObject::tr("Clear Azure OpenAI GPT3.5 API Key"),
                  executablePath,
                  QStringLiteral("-clearauzre35key %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Clear Azure OpenAI GPT4 API Key"),
                  executablePath,
                  QStringLiteral("-clearazure4key %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Clear Gemini Pro API Key"),
                  executablePath,
                  QStringLiteral("-cleargeminiprokey %1").arg(keyword),
                  QStringLiteral(":/rc/images/geminipro.png")},
        {QObject::tr("Clear Azure OpenAI GPT3.5 API Endpoint"),
                  executablePath,
                  QStringLiteral("-clearazure35ep %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Clear Azure OpenAI GPT4 API Endpoint"),
                  executablePath,
                  QStringLiteral("-clearazure4ep %1").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
    };
    return output(items);
}

bool askHandler(const QString &keyword)
{
    auto                       executablePath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    std::vector<GPTActionItem> items          = {
        {QObject::tr("Ask OpenAI GPT3.5"), executablePath, QStringLiteral("-askgpt35 \"%1\"").arg(keyword), QStringLiteral(":/rc/images/openai.png")},
        {QObject::tr("Ask OpenAI GPT4"), executablePath, QStringLiteral("-askgpt4 \"%1\"").arg(keyword), QStringLiteral(":/rc/images/openai.png")},
        {QObject::tr("Ask Azure OpenAI GPT3.5"),
                  executablePath,
                  QStringLiteral("-askazure35 \"%1\"").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Ask Azure OpenAI GPT4"),
                  executablePath,
                  QStringLiteral("-askazure4 \"%1\"").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Ask Gemini Pro"),
                  executablePath,
                  QStringLiteral("-askgeminipro \"%1\"").arg(keyword),
                  QStringLiteral(":/rc/images/geminipro.png")},
    };
    return output(items);
}

bool translateHandler(const QString &keyword)
{
    auto                       executablePath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    std::vector<GPTActionItem> items          = {
        {QObject::tr("Translate by OpenAI GPT3.5"),
                  executablePath,
                  QStringLiteral("-transgpt35 \"%1\"").arg(keyword),
                  QStringLiteral(":/rc/images/openai.png")},
        {QObject::tr("Translate by OpenAI GPT4"),
                  executablePath,
                  QStringLiteral("-transgpt4 \"%1\"").arg(keyword),
                  QStringLiteral(":/rc/images/openai.png")},
        {QObject::tr("Translate by Azure OpenAI GPT3.5"),
                  executablePath,
                  QStringLiteral("-transazure4 \"%1\"").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Translate by Azure OpenAI GPT4"),
                  executablePath,
                  QStringLiteral("-transazure4 \"%1\"").arg(keyword),
                  QStringLiteral(":/rc/images/azure.png")},
        {QObject::tr("Translate by Gemini Pro"),
                  executablePath,
                  QStringLiteral("-transgeminipro \"%1\"").arg(keyword),
                  QStringLiteral(":/rc/images/geminipro.png")},
    };
    return output(items);
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
    SharedTools::QtSingleApplication app("ChatGPT", argc, argv);

    QCoreApplication::setApplicationName("ChatGPT");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationDomain("ismisv.com");
    QCoreApplication::setOrganizationName("Derfla");

    if (argc <= 2)
    {
        QTextStream textStream(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        textStream.setCodec("UTF-8");
#else
        textStream.setEncoding(QStringConverter::Utf8);
#endif

        textStream << "invalid arguments";
        return 1;
    }

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

    if (!translator.load("chatgpt_" + locale, localeDirPath))
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

    QString                                             cmd(argv[1]);
    QMap<QString, std::function<bool(const QString &)>> cmdMap = {
        {"setkey", std::bind(&setAPIKeyHandler, std::placeholders::_1)},
        {"setapi", std::bind(&setAPIKeyHandler, std::placeholders::_1)},
        {"apikey", std::bind(&clearSetAPIKeyHandler, std::placeholders::_1)},
        {"set", std::bind(&setAPIKeyEndpointHandler, std::placeholders::_1)},
        {"setendpoint", std::bind(&setAPIEndpointHandler, std::placeholders::_1)},
        {"clearendpoint", std::bind(&clearAPIEndpointHandler, std::placeholders::_1)},
        {"clrendpoint", std::bind(&clearAPIEndpointHandler, std::placeholders::_1)},
        {"endpoint", std::bind(&clearSetAPIEndpointHandler, std::placeholders::_1)},
        {"clrkey", std::bind(&clearAPIKeyHandler, std::placeholders::_1)},
        {"clrapi", std::bind(&clearAPIKeyHandler, std::placeholders::_1)},
        {"clr", std::bind(&clearAPIKeyEndpointHandler, std::placeholders::_1)},
        {"clear", std::bind(&clearAPIKeyEndpointHandler, std::placeholders::_1)},
        {"ask", std::bind(&askHandler, std::placeholders::_1)},
        {"translate", std::bind(&translateHandler, std::placeholders::_1)},
        {"trans", std::bind(&translateHandler, std::placeholders::_1)},
    };
    auto iter = cmdMap.find(cmd);
    if (cmdMap.end() != iter)
    {
        auto    func = iter.value();
        QString keyword;
#if defined(Q_OS_WIN)
        int nArgs = 0;

        LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
        for (int i = 2; i < argc; i++)
        {
            keyword.append(QString::fromWCharArray(szArglist[i]));
            keyword.append(' ');
        }
        LocalFree(szArglist);
#else
        for (int i = 2; i < argc; i++)
        {
            keyword.append(argv[i]);
            keyword.append(' ');
        }
#endif
        if (!func(keyword.trimmed()))
        {
            return 1;
        }
    }

    return 0;
}
