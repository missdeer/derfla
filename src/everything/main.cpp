#include "stdafx.h"
#include "qtsingleapplication.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <QIcon>
#include "util.h"
#include "everythingwrapper.h"

using namespace rapidjson;

static const int maxCount = 25;

bool handleFile(const QString& pattern)
{
	QTextStream ts(stdout);
	QStringList res;
	if (!QuickGetFilesByFileName(pattern, res, [](bool isDir){return !isDir; }, maxCount))
	{
		ts << "searching " << pattern << "by everything failed.";
		return false;
	}

    Document d;
    d.Parse("[]");
    Q_ASSERT(d.IsArray());
    Document::AllocatorType& a = d.GetAllocator();
    for (const auto& f : res)
    {
        QFileInfo fi(f);

        Value o(kObjectType);
        o.AddMember(Value("title", a), Value(fi.fileName().toStdString().c_str(), a), a);
        o.AddMember(Value("description", a), Value(QDir::toNativeSeparators(fi.absolutePath()).toStdString().c_str(), a), a);
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

    ts << QString(buffer.GetString());
    return true;
}

bool handleDir(const QString& pattern)
{
	QTextStream ts(stdout);
	QStringList res;
	if (!QuickGetFilesByFileName(pattern, res, [](bool isDir){return isDir; }, maxCount))
	{
		ts << "searching " << pattern << "by everything failed.";
		return false;
	}

    Document d;
    d.Parse("[]");
    Q_ASSERT(d.IsArray());
    Document::AllocatorType& a = d.GetAllocator();
    for (const auto& f : res)
    {
        QFileInfo fi(f);

        Value o(kObjectType);
        o.AddMember(Value("title", a), Value(fi.fileName().toStdString().c_str(), a), a);
        o.AddMember(Value("description", a), Value(QDir::toNativeSeparators(fi.absolutePath()).toStdString().c_str(), a), a);
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

    ts << QString(buffer.GetString());
    return true;
}

bool handleVSOpen(const QString& pattern)
{
	QTextStream ts(stdout);
	QStringList res;
	if (!QuickGetFilesByFileName(pattern, res, [](bool isDir){return !isDir; }, maxCount))
	{
		ts << "searching " << pattern << "by everything failed.";
		return false;
	}

    QFile f(":/open-in-vs.vbs");
    if (!f.open(QIODevice::ReadOnly))
        return false;
    QByteArray c = f.readAll();
    f.close();

    Document d;
    d.Parse("[]");
    Q_ASSERT(d.IsArray());
    Document::AllocatorType& a = d.GetAllocator();
    for (const auto& f : res)
    {
        QFileInfo fi(f);
        if (fi.isDir())
            continue;

        QStringList args;
        args << QDir::toNativeSeparators(fi.absoluteFilePath()) << "1" << "1";
        Value o(kObjectType);
        o.AddMember(Value("title", a), Value(fi.fileName().toStdString().c_str(), a), a);
        o.AddMember(Value("description", a), Value(QDir::toNativeSeparators(fi.absolutePath()).toStdString().c_str(), a), a);
        o.AddMember(Value("target", a), Value(QString(c).toStdString().c_str(), a), a);
        o.AddMember(Value("arguments", a), Value(args.join(QChar(' ')).toStdString().c_str(), a), a);
        o.AddMember(Value("workingDir", a), Value(QDir::toNativeSeparators(fi.absolutePath()).toStdString().c_str(), a), a);
        o.AddMember(Value("actionType", a), Value("script", a), a);
        o.AddMember(Value("scriptExecutor", a), Value("cscript", a), a);
        QByteArray bytes = util::extractPNGIconFromFile(fi);
        o.AddMember(Value("iconData", a), Value(QString(bytes.toBase64()).toStdString().c_str(), a), a);
        d.PushBack(o, a);
    }
    StringBuffer buffer;
    Writer<StringBuffer, Document::EncodingType, ASCII<>> writer(buffer);
    d.Accept(writer);

    ts << QString(buffer.GetString());
    return true;
}

bool handleShellOpen(const QString& pattern)
{
	QTextStream ts(stdout);
	QStringList res;
	if (!QuickGetFilesByFileName(pattern, res, [](bool){ return true; }, maxCount))
	{
		ts << "searching " << pattern << "by everything failed.";
		return false;
	}

    Document d;
    d.Parse("[]");
    Q_ASSERT(d.IsArray());
    Document::AllocatorType& a = d.GetAllocator();
    for (const auto& f : res)
    {
        QFileInfo fi(f);

        Value o(kObjectType);
        o.AddMember(Value("title", a), Value(fi.fileName().toStdString().c_str(), a), a);
        o.AddMember(Value("description", a), Value(QDir::toNativeSeparators(fi.absolutePath()).toStdString().c_str(), a), a);
        o.AddMember(Value("target", a), Value(QDir::toNativeSeparators(fi.absoluteFilePath()).toStdString().c_str(), a), a);
        o.AddMember(Value("arguments", a), Value("", a), a);
        o.AddMember(Value("workingDir", a), Value(QDir::toNativeSeparators(fi.absolutePath()).toStdString().c_str(), a), a);
        o.AddMember(Value("actionType", a), Value("shellExecute", a), a);
        if (fi.isDir())
        {
            o.AddMember(Value("iconPath", a), Value(QString(qApp->applicationDirPath() % "/folder.png").toStdString().c_str(), a), a);
        }
        else
        {
            QByteArray bytes = util::extractPNGIconFromFile(fi);
            o.AddMember(Value("iconData", a), Value(QString(bytes.toBase64()).toStdString().c_str(), a), a);
        }
        d.PushBack(o, a);
    }
    StringBuffer buffer;
    Writer<StringBuffer, Document::EncodingType, ASCII<>> writer(buffer);
    d.Accept(writer);

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

    if (argc == 3 || argc == 4)
    {
        QTextStream ts( stdout );
        if (!isEverythingRunning())
        {
            ts << "everything util is not running.";
            return 1;
        }
        QString pattern(argc == 3 ? argv[2] : argv[3]);
        if (pattern.size() < 2)
        {
            ts << "[]";
            return 0;
        }

		if (argc == 4)
		{
			QString options(argv[2]);
			Everything_SetMatchWholeWord(options.contains(QChar('w'), Qt::CaseInsensitive));
			Everything_SetMatchCase(options.contains(QChar('c'), Qt::CaseInsensitive));
			Everything_SetRegex(options.contains(QChar('r'), Qt::CaseInsensitive));
		}

        QMap<QString, std::function<bool(const QString&)>> m = {
        { "f", std::bind(&handleFile, std::placeholders::_1) },
        { "d", std::bind(&handleDir, std::placeholders::_1) },
        { "vs", std::bind(&handleVSOpen, std::placeholders::_1) },
        { "open", std::bind(&handleShellOpen, std::placeholders::_1) },
    };
        QString cmd(argv[1]);
        auto it = m.find(cmd);
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
