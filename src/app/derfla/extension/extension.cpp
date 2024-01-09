#include "stdafx.h"

#include "extension.h"
#include "scopedguard.h"

Extension::Extension(QObject *parent) : QObject(parent) {}

Extension::~Extension()
{
    stopQuery();
    if (daemon_)
    {
        stopDaemon();
    }
}

void Extension::runDaemon()
{
    QStringList arguments;
    QString     program = executable_;
    if (!executor_.isEmpty())
    {
        program = QStandardPaths::findExecutable(executor_);
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
        program = QStandardPaths::findExecutable(executor_);
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
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Extension::finished);
    if (executor_.isEmpty())
    {
        process->setProgram(executable_);
    }
    else
    {
        process->setProgram(QStandardPaths::findExecutable(executor_));
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

void Extension::parseActionConfig(QJsonObject &eleObj, DerflaActionPtr &action)
{
    auto setObjectValue = [&eleObj](const QString &key, const std::function<void(const QString &)> &setter) {
        if (eleObj.contains(key) && eleObj[key].isString())
        {
            setter(eleObj[key].toString());
        }
    };

    setObjectValue(QStringLiteral("title"), [&action](const QString &value) { action->setTitle(value); });
    setObjectValue(QStringLiteral("description"), [&action](const QString &value) { action->setDescription(value); });
    setObjectValue(QStringLiteral("target"), [&action](const QString &value) { action->setTarget(value); });
    setObjectValue(QStringLiteral("arguments"), [&action](const QString &value) { action->setArguments(value); });
    setObjectValue(QStringLiteral("workingDir"), [&action](const QString &value) { action->setWorkingDirectory(value); });
    setObjectValue(QStringLiteral("actionType"), [&action](const QString &value) { action->setActionType(value); });
    setObjectValue(QStringLiteral("scriptExecutor"), [&action](const QString &value) { action->setScriptExecutor(value); });
    setObjectValue(QStringLiteral("iconPath"), [&action](const QString &value) { action->setIcon(QIcon(value)); });
    setObjectValue(QStringLiteral("iconData"), [&action](const QString &value) {
        auto    iconData = QByteArray::fromBase64(value.toUtf8());
        QPixmap pixmap;
        if (pixmap.loadFromData(iconData))
        {
            action->setIcon(QIcon(pixmap));
        }
    });
}
void Extension::finished(int exitCode, QProcess::ExitStatus /*exitStatus*/)
{
    auto *process = qobject_cast<QProcess *>(sender());

    process->deleteLater();

    if (process != subProcess_)
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

    auto output = process->readAllStandardOutput();
    // convert json output to action list
    QJsonParseError error;
    auto            doc = QJsonDocument::fromJson(output, &error);
    qDebug() << error.errorString();
    if (error.error == QJsonParseError::NoError && doc.isArray())
    {
        auto array = doc.array();
        for (auto arrEle : array)
        {
            if (!arrEle.isObject())
            {
                continue;
            }
            QJsonObject     eleObj = arrEle.toObject();
            DerflaActionPtr action(new DerflaAction);
            parseActionConfig(eleObj, action);
            derflaActions_.push_back(action);
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

QProcessEnvironment Extension::getProcessEnvironment()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

#if defined(Q_OS_WIN)
    auto pathEnv = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()) + ";" + qgetenv("PATH");
    env.insert("PATH", pathEnv.toUtf8()); // so that extensions can use Derfla main executable's Qt binaries
    env.insert("QT_PLUGIN_PATH", QDir::toNativeSeparators(QCoreApplication::applicationDirPath()).toUtf8());
#elif defined(Q_OS_MAC)
    QDir dir(QCoreApplication::applicationDirPath());
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
