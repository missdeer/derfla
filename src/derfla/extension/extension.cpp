#include "stdafx.h"

#include "extension.h"
#include "scopedguard.h"

Extension::Extension(QObject *parent) : QObject(parent) {}

Extension::~Extension()
{
    stopQuery();
    if (daemon_)
        stopDaemon();
}

void Extension::runDaemon()
{
    QStringList arguments;
    QString     program = executable_;
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
    QString     program = executable_;
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

bool Extension::query(const QString &input)
{
    QStringList arguments;
    auto       *process = new QProcess;
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
    if (executor_.isEmpty())
    {
        process->setProgram(executable_);
    }
    else
    {
        process->setProgram(findProgram());
        arguments << executable_;
    }
    process->setProcessEnvironment(getProcessEnvironment());
    process->setWorkingDirectory(QFileInfo(executable_).absolutePath());
    arguments << input.split(QChar(' '));
    process->setArguments(arguments);
    process->start();
    subProcess_ = process;
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
    QPixmap    pixmap;
    if (pixmap.loadFromData(c))
    {
        waitIcon_ = QIcon(pixmap);
    }
}

void Extension::finished(int exitCode, QProcess::ExitStatus /*exitStatus*/)
{
    auto *p = qobject_cast<QProcess *>(sender());

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
    QJsonDocument   doc = QJsonDocument::fromJson(output, &error);
    qDebug() << error.errorString();
    if (error.error == QJsonParseError::NoError && doc.isArray())
    {
        QJsonArray array = doc.array();
        for (auto arrEle : array)
        {
            if (!arrEle.isObject())
            {
                continue;
            }
            QJsonObject     eleObj = arrEle.toObject();
            DerflaActionPtr action(new DerflaAction);
            if (eleObj["title"].isString())
            {
                action->setTitle(eleObj["title"].toString());
            }
            if (eleObj["description"].isString())
            {
                action->setDescription(eleObj["description"].toString());
            }
            if (eleObj["target"].isString())
            {
                action->setTarget(eleObj["target"].toString());
            }
            if (eleObj["arguments"].isString())
            {
                action->setArguments(eleObj["arguments"].toString());
            }
            if (eleObj["workingDir"].isString())
            {
                action->setWorkingDirectory(eleObj["workingDir"].toString());
            }
            if (eleObj["actionType"].isString())
            {
                action->setActionType(eleObj["actionType"].toString());
            }
            if (eleObj["scriptExecutor"].isString())
            {
                action->setScriptExecutor(eleObj["scriptExecutor"].toString());
            }
            if (eleObj["iconPath"].isString())
            {
                action->setIcon(QIcon(eleObj["iconPath"].toString()));
            }
            if (eleObj["iconData"].isString())
            {
                QByteArray iconData = QByteArray::fromBase64(eleObj["iconData"].toString().toUtf8());
                QPixmap    pixmap;
                if (pixmap.loadFromData(iconData))
                {
                    action->setIcon(QIcon(pixmap));
                }
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
    QString     path        = qgetenv("PATH");
    QStringList environment = QProcess::systemEnvironment();
    auto        iter        = std::find_if(environment.begin(), environment.end(), [&](const QString &env) { return env.startsWith("PATH="); });
    if (environment.end() != iter)
    {
        path = iter->mid(5);
    }
#if defined(Q_OS_WIN)
    QString exe = executor_ % ".exe";
    envPaths << path.split(QChar(';'));
#else
    QString exe = executor_;
    envPaths << path.split(QChar(':'));
#endif

    iter = std::find_if(envPaths.begin(), envPaths.end(), [&exe](const QString &p) { return QFile::exists(p % exe); });
    if (envPaths.end() == iter)
    {
        qDebug() << "can't find program:" << exe;
        return {};
    }
    return *iter % exe;
}

QProcessEnvironment Extension::getProcessEnvironment()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

#if defined(Q_OS_WIN)
    auto pathEnv = QDir::toNativeSeparators(QApplication::applicationDirPath()) + ";" + qgetenv("PATH");
    env.insert("PATH", pathEnv.toUtf8()); // so that extensions can use Derfla main executable's Qt binaries
    env.insert("QT_PLUGIN_PATH", QDir::toNativeSeparators(QApplication::applicationDirPath()).toUtf8());
#elif defined(Q_OS_MAC)
    QDir dir(QApplication::applicationDirPath());
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
