#include "stdafx.h"
#include "qtsingleapplication.h"
#include <QIcon>
#include "util.h"

typedef std::tuple<QString, QString, QString> SearchItem;

bool output(const std::vector<SearchItem>& items)
{
    QJsonDocument d = QJsonDocument::fromJson("[]");
    Q_ASSERT(d.isArray());
    QJsonArray arr = d.array();
    for (auto & i : items)
    {
        QVariantMap m;
        m.insert("title", std::get<0>(i) );
        m.insert("target", std::get<1>(i) );
        m.insert("actionType", "openUrl");
        QFile icon(std::get<2>(i) );
        if (icon.open(QIODevice::ReadOnly))
        {
            auto bytes = icon.readAll();
            icon.close();
            m.insert("iconData", bytes.toBase64());
        }
        arr.append(QJsonObject::fromVariantMap(m));
    }

    d.setArray(arr);
    QTextStream ts(stdout);
    ts.setCodec("UTF-8");
    ts << QString(d.toJson(QJsonDocument::Compact));
    return true;
}

bool googleHandler(const QString& keyword)
{
    QString url = QString("https://www.google.com/search?q=%1&ie=utf-8&oe=utf-8").arg(QString(keyword.toUtf8().toPercentEncoding()));
    auto item = std::make_tuple(QString("Search \"%1\" on Google").arg(keyword), url, ":/rc/images/google.png");
    std::vector<SearchItem> items = { item };
    return output(items);
}

bool bingHandler(const QString& keyword)
{
    QString url = QString("https://www.bing.com/search?q=%1").arg(QString(keyword.toUtf8().toPercentEncoding()));
    auto item = std::make_tuple(QString("Search \"%1\" on Bing").arg(keyword), url, ":/rc/images/bing.png");
    std::vector<SearchItem> items = { item };
    return output(items);
}

bool baiduHandler(const QString& keyword)
{
    QString url = QString("https://www.baidu.com/s?wd=%1").arg(QString(keyword.toUtf8().toPercentEncoding()));
    auto item = std::make_tuple(QString("Search \"%1\" on Baidu").arg(keyword), url, ":/rc/images/baidu.png");
    std::vector<SearchItem> items = { item };
    return output(items);
}

bool duckduckgoHandler(const QString& keyword)
{
    QString url = QString("https://duckduckgo.com/?q=%1&ia=web").arg(QString(keyword.toUtf8().toPercentEncoding()));
    auto item = std::make_tuple(QString("Search \"%1\" on Duckduckgo").arg(keyword), url, ":/rc/images/duckduckgo.png");
    std::vector<SearchItem> items = { item };
    return output(items);
}

bool wikipediaHandler(const QString& keyword)
{
    QString url = QString("https://en.wikipedia.org/wiki/%1").arg(QString(keyword.toUtf8().toPercentEncoding()));
    auto item = std::make_tuple(QString("Search \"%1\" on Wikipedia").arg(keyword), url, ":/rc/images/wikipedia.png");
    std::vector<SearchItem> items = { item };
    return output(items);
}

bool allHandler(const QString& keyword)
{
    auto googleItem = std::make_tuple(QString("Search \"%1\" on Google").arg(keyword), 
        QString("https://www.google.com/search?q=%1&ie=utf-8&oe=utf-8").arg(QString(keyword.toUtf8().toPercentEncoding())),
        ":/rc/images/google.png");
    auto bingItem = std::make_tuple(QString("Search \"%1\" on Bing").arg(keyword), 
        QString("https://www.bing.com/search?q=%1").arg(QString(keyword.toUtf8().toPercentEncoding())),
        ":/rc/images/bing.png");
    auto baiduItem = std::make_tuple(QString("Search \"%1\" on Baidu").arg(keyword),
        QString("https://www.baidu.com/s?wd=%1").arg(QString(keyword.toUtf8().toPercentEncoding())),
        ":/rc/images/baidu.png");
    auto duckItem = std::make_tuple(QString("Search \"%1\" on Duckduckgo").arg(keyword), 
        QString("https://duckduckgo.com/?q=%1&ia=web").arg(QString(keyword.toUtf8().toPercentEncoding())),
        ":/rc/images/duckduckgo.png");
    auto wikiItem = std::make_tuple(QString("Search \"%1\" on Wikipedia").arg(keyword),
        QString("https://en.wikipedia.org/wiki/%1").arg(QString(keyword.toUtf8().toPercentEncoding())),
        ":/rc/images/wikipedia.png");
    std::vector<SearchItem> items = { googleItem, bingItem,baiduItem, duckItem, wikiItem };
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
    SharedTools::QtSingleApplication a("WebSearch", argc, argv);

    a.setApplicationName("WebSearch");
    a.setApplicationVersion("1.0");
    a.setOrganizationDomain("dfordsoft.com");
    a.setOrganizationName("Derfla");

    if (argc <= 2)
    {
        QTextStream ts(stdout);
        ts.setCodec("UTF-8");

        ts << "invalid arguments";
        return 1;
    }

    QString se(argv[1]);
    QMap<QString, std::function<bool(const QString&)>> m = {
    { "s",          std::bind(&allHandler,        std::placeholders::_1) },
    { "se",         std::bind(&allHandler,        std::placeholders::_1) },
    { "search",     std::bind(&allHandler,        std::placeholders::_1) },
    { "g",          std::bind(&googleHandler,     std::placeholders::_1) },
    { "gg",         std::bind(&googleHandler,     std::placeholders::_1) },
    { "google",     std::bind(&googleHandler,     std::placeholders::_1) },
    { "b",          std::bind(&bingHandler,       std::placeholders::_1) },
    { "bing",       std::bind(&bingHandler,       std::placeholders::_1) },
    { "baidu",      std::bind(&baiduHandler,      std::placeholders::_1) },
    { "d",          std::bind(&duckduckgoHandler, std::placeholders::_1) },
    { "duck",       std::bind(&duckduckgoHandler, std::placeholders::_1) },
    { "ddgo",       std::bind(&duckduckgoHandler, std::placeholders::_1) },
    { "duckduckgo", std::bind(&duckduckgoHandler, std::placeholders::_1) },
    { "w",          std::bind(&wikipediaHandler,  std::placeholders::_1) },
    { "wiki",       std::bind(&wikipediaHandler,  std::placeholders::_1) },
    { "wikipedia",  std::bind(&wikipediaHandler,  std::placeholders::_1) },
};
    auto it = m.find(se);
    if (m.end() != it)
    {
        auto f = it.value();
        QString keyword;
#if defined(Q_OS_WIN)
        int nArgs = 0;

        LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
        for (int i = 2; i < argc; i++)
        {
            keyword.append(QString::fromWCharArray(szArglist[i]));
            keyword.append(' ');
        }
#else
        for (int i = 2; i < argc; i++)
        {
            keyword.append(argv[i]);
            keyword.append(' ');
        }
#endif
        if (!f(keyword.trimmed()))
            return 1;
    }

    return 0;
}
