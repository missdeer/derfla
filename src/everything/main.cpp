#include "stdafx.h"
#include "qtsingleapplication.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <QIcon>
#include "util.h"
#include "everythingwrapper.h"

using namespace rapidjson;

bool handleFile(const QStringList& res)
{
    Document d;
    d.Parse("[]");
    Q_ASSERT(d.IsArray());
    Document::AllocatorType& a = d.GetAllocator();
    for (const auto& f : res)
    {
        QFileInfo fi(f);
        if (!fi.isFile())
            continue;

        Value o(kObjectType);
        o.AddMember(Value("title", a), Value(fi.fileName().toStdString().c_str(), a), a);
        o.AddMember(Value("description", a), Value(fi.absolutePath().toStdString().c_str(), a), a);
        o.AddMember(Value("target", a), Value(QDir::toNativeSeparators(fi.absoluteFilePath()).toStdString().c_str(), a), a);
        o.AddMember(Value("arguments", a), Value("", a), a);
        o.AddMember(Value("workingDir", a), Value("", a), a);
        o.AddMember(Value("actionType", a), Value("revealFile", a), a);
        QByteArray bytes = util::extractPNGIconFromFile(fi);
        o.AddMember(Value("iconData", a), Value(QString(bytes.toBase64()).toStdString().c_str(), a), a);
        d.PushBack(o, a);
    }
    StringBuffer buffer;
    Writer<StringBuffer, Document::EncodingType, ASCII<>> writer(buffer);
    d.Accept(writer);

    QTextStream ts( stdout );
    ts << QString(buffer.GetString());
    return true;
}

bool handleDir(const QStringList& res)
{
    Document d;
    d.Parse("[]");
    Q_ASSERT(d.IsArray());
    Document::AllocatorType& a = d.GetAllocator();
    for (const auto& f : res)
    {
        QFileInfo fi(f);
        if (!fi.isDir())
            continue;

        Value o(kObjectType);
        o.AddMember(Value("title", a), Value(fi.fileName().toStdString().c_str(), a), a);
        o.AddMember(Value("description", a), Value(fi.absolutePath().toStdString().c_str(), a), a);
        o.AddMember(Value("target", a), Value(QDir::toNativeSeparators(fi.absoluteFilePath()).toStdString().c_str(), a), a);
        o.AddMember(Value("arguments", a), Value("", a), a);
        o.AddMember(Value("workingDir", a), Value("", a), a);
        o.AddMember(Value("actionType", a), Value("shellExecute", a), a);
        o.AddMember(Value("iconPath", a), Value(QString(qApp->applicationDirPath() % "/folder.png").toStdString().c_str(), a), a);
        d.PushBack(o, a);
    }
    StringBuffer buffer;
    Writer<StringBuffer, Document::EncodingType, ASCII<>> writer(buffer);
    d.Accept(writer);

    QTextStream ts( stdout );
    ts << QString(buffer.GetString());
    return true;
}

bool handleVSOpen(const QStringList& res)
{
    return true;
}

bool handleShellOpen(const QStringList& res)
{
    Document d;
    d.Parse("[]");
    Q_ASSERT(d.IsArray());
    Document::AllocatorType& a = d.GetAllocator();
    for (const auto& f : res)
    {
        QFileInfo fi(f);

        Value o(kObjectType);
        o.AddMember(Value("title", a), Value(fi.fileName().toStdString().c_str(), a), a);
        o.AddMember(Value("description", a), Value(fi.absolutePath().toStdString().c_str(), a), a);
        o.AddMember(Value("target", a), Value(QDir::toNativeSeparators(fi.absoluteFilePath()).toStdString().c_str(), a), a);
        o.AddMember(Value("arguments", a), Value("", a), a);
        o.AddMember(Value("workingDir", a), Value(QDir::toNativeSeparators(fi.absolutePath()).toStdString().c_str(), a), a);
        o.AddMember(Value("actionType", a), Value("shellExecute", a), a);
        QByteArray bytes = util::extractPNGIconFromFile(fi);
        o.AddMember(Value("iconData", a), Value(QString(bytes.toBase64()).toStdString().c_str(), a), a);
        d.PushBack(o, a);
    }
    StringBuffer buffer;
    Writer<StringBuffer, Document::EncodingType, ASCII<>> writer(buffer);
    d.Accept(writer);

    QTextStream ts( stdout );
    ts << QString(buffer.GetString());
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
    a.setOrganizationDomain("dfordsoft.com");
    a.setOrganizationName("Derfla");

    if (argc == 2)
    {
        QTextStream ts( stdout );
        if (!isEverythingRunning())
        {
            ts << "everything util is not running.";
            return 1;
        }
        QStringList res;
        if (!QuickGetFilesByFileName(QString(argv[2]), res))
        {
            ts << "searching " << argv[1] << "by everything failed.";
            return 2;
        }

        QMap<QString, std::function<bool(const QStringList&)>> m = {
        { "f", std::bind(&handleFile, std::placeholders::_1) },
        { "d", std::bind(&handleDir, std::placeholders::_1) },
        { "vs", std::bind(&handleVSOpen, std::placeholders::_1) },
        { "open", std::bind(&handleShellOpen, std::placeholders::_1) },
    };
        auto it = m.find(QString(argv[1]));
        if (m.end() == it)
        {
            ts << "unsupported query action";
            return 3;
        }
        auto f = m[QString(argv[1])];
        if (!f(res))
        {
            ts << "handler failed";
            return 4;
        }
    }

    return 0;
}
