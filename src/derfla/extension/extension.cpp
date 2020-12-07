#include "stdafx.h"
#include "scopedguard.h"
#include "extension.h"

Extension::Extension(QObject *parent) 
    : QObject(parent)
    , daemon_(false)
    , subProcess_(nullptr)
{
}

Extension::~Extension()
{
    stopQuery();
    if (daemon_)
        stopDaemon();
}

void Extension::runDaemon()
{
    QStringList arguments;
    QString program = executable_;
    if (!executor_.isEmpty())
    {
        program = findProgram();
        arguments << executable_;
    }

    QProcess proc;
    proc.setProgram(program);
    proc.setProcessEnvironment(getProcessEnvironment());
    proc.setArguments(arguments);
    proc.setWorkingDirectory(QFileInfo(executable_).absolutePath());
    proc.startDetached();
}

void Extension::stopDaemon()
{
    QStringList arguments;
    QString program = executable_;
    if (!executor_.isEmpty())
    {
        program = findProgram();
        arguments << executable_;
    }
    arguments << "/exit";

    QProcess proc;
    proc.setProgram(program);
    proc.setProcessEnvironment(getProcessEnvironment());
    proc.setArguments(arguments);
    proc.setWorkingDirectory(QFileInfo(executable_).absolutePath());
    proc.startDetached();
}

bool Extension::query(const QString& input)
{
    QStringList arguments;
    QProcess *p = new QProcess;
    connect(p, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
    if (executor_.isEmpty())
    {
        p->setProgram(executable_);
    }
    else
    {
        p->setProgram(findProgram());
        arguments << executable_;
    }
    p->setProcessEnvironment(getProcessEnvironment());
    p->setWorkingDirectory(QFileInfo(executable_).absolutePath());
    arguments << input.split(QChar(' '));
    p->setArguments(arguments);
    p->start();
    subProcess_ = p;
    return true;
}

void Extension::stopQuery()
{
    // discard output from previous processes
    subProcess_ = nullptr;
}

const QString &Extension::author() const
{
    return author_;
}

void Extension::setAuthor(const QString &author)
{
    author_ = author;
}

const QString &Extension::name() const
{
    return name_;
}

void Extension::setName(const QString &name)
{
    name_ = name;
}

const QString &Extension::description() const
{
    return description_;
}

void Extension::setDescription(const QString &description)
{
    description_ = description;
}

const QString &Extension::executable() const
{
    return executable_;
}

void Extension::setExecutable(const QString &executable)
{
    executable_ = executable;
}

const QString &Extension::executor() const
{
    return executor_;
}

void Extension::setExecutor(const QString &executor)
{
    executor_ = executor;
}

void Extension::setPrefix(const QStringList &prefix)
{
    prefix_ = prefix;
}

const QStringList &Extension::prefix()
{
    return prefix_;
}

const QString &Extension::waitTitle() const
{
    return waitTitle_;
}

void Extension::setWaitTitle(const QString &waitTitle)
{
    waitTitle_ = waitTitle;
}

const QString &Extension::waitDescription() const
{
    return waitDescription_;
}

void Extension::setWaitDescription(const QString &waitDescription)
{
    waitDescription_ = waitDescription;
}

const QIcon &Extension::waitIcon() const
{
    return waitIcon_;
}

void Extension::setWaitIcon(const QIcon &waitIcon)
{
    waitIcon_ = waitIcon;
}

void Extension::setWaitIconPath(const QString &waitIconPath)
{
    QPixmap pixmap;
    if (pixmap.load(waitIconPath))
    {
        waitIcon_ = QIcon(pixmap);
    }
}

void Extension::setWaitIconData(const QString &waitIconData)
{
    QByteArray c = QByteArray::fromBase64(waitIconData.toUtf8());
    QPixmap pixmap;
    if (pixmap.loadFromData(c))
    {
        waitIcon_ = QIcon(pixmap);
    }
}

void Extension::finished(int exitCode, QProcess::ExitStatus /*exitStatus*/)
{
    QProcess* p = qobject_cast<QProcess*>(sender());

    p->deleteLater();

    if (p != subProcess_)
    {
        return;
    }

    derflaActions_.clear();
    // it's the last one
    subProcess_ = nullptr;
    if (exitCode != 0)
    {
        emit queried(derflaActions_);
        return;
    }

    QByteArray output = p->readAllStandardOutput();
    // convert json output to action list
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(output, &error);
    qDebug() << error.errorString();
    if (doc.isArray())
    {
        QJsonArray array = doc.array();
        for (auto a : array)
        {
            if (!a.isObject())
                continue;
            QJsonObject o = a.toObject();
            DerflaActionPtr action(new DerflaAction);
            if (o["title"].isString())
                action->setTitle(o["title"].toString());
            if (o["description"].isString())
                action->setDescription(o["description"].toString());
            if (o["target"].isString())
                action->setTarget(o["target"].toString());
            if (o["arguments"].isString())
                action->setArguments(o["arguments"].toString());
            if (o["workingDir"].isString())
                action->setWorkingDirectory(o["workingDir"].toString());
            if (o["actionType"].isString())
                action->setActionType(o["actionType"].toString());
            if (o["scriptExecutor"].isString())
                action->setScriptExecutor(o["scriptExecutor"].toString());
            if (o["iconPath"].isString())
                action->setIcon(QIcon(o["iconPath"].toString()));
            if (o["iconData"].isString())
            {
                QByteArray c = QByteArray::fromBase64(o["iconData"].toString().toUtf8());
                QPixmap pixmap;
                if (pixmap.loadFromData(c))
                    action->setIcon(QIcon(pixmap));
            }
            derflaActions_.append(action);
        }
    }

    emit queried(derflaActions_);
}

bool Extension::daemon() const
{
    return daemon_;
}

void Extension::setDaemon(bool daemon)
{
    daemon_ = daemon;
}

const QString &Extension::id() const
{
    return id_;
}

void Extension::setId(const QString &id)
{
    id_ = id;
}

QString Extension::findProgram()
{
    QStringList envPaths;
    QString path = qgetenv("PATH");
    QStringList environment = QProcess::systemEnvironment();
    auto it = std::find_if(environment.begin(), environment.end(),
                           [&](const QString& env) { return env.startsWith("PATH="); });
    if (environment.end() != it)
        path = it->mid(5);
#if defined(Q_OS_WIN)
    QString exe = executor_ % ".exe";
    envPaths << path.split(QChar(';'));
#else
    QString exe = executor_;
    envPaths << path.split(QChar(':'));
#endif

    it = std::find_if(envPaths.begin(), envPaths.end(), [&exe](const QString& p) {
        return QFile::exists(p % exe);
    });
    if (envPaths.end() == it)
    {
        qDebug() << "can't find program:" << exe;
        return "";
    }
    return *it % exe;
}

QProcessEnvironment Extension::getProcessEnvironment()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

#if defined(Q_OS_WIN)
    auto pathEnv = QDir::toNativeSeparators(QApplication::applicationDirPath()) + ";" + qgetenv("PATH");
    env.insert("PATH", pathEnv.toUtf8()); // so that extensions can use Derfla main executable's Qt binaries
    env.insert("QT_PLUGIN_PATH", QDir::toNativeSeparators(QApplication::applicationDirPath()).toUtf8());
#elif defined(Q_OS_MAC)
    QDir dir(a.applicationDirPath());
    dir.cdUp();
    dir.cd("Libs");
    auto pathEnv = dir.absolutePath() + ":" + qgetenv("DYLD_LIBRARY_PATH");
    env.insert("DYLD_LIBRARY_PATH", pathEnv.toUtf8());
    env.insert("QT_MAC_DISABLE_FOREGROUND_APPLICATION_TRANSFORM", "1");
#else
    auto pathEnv = dir.absolutePath() + ":" + qgetenv("LD_LIBRARY_PATH");
    env.insert("LD_LIBRARY_PATH", pathEnv.toUtf8());
#endif
    return env;
}
