#include "stdafx.h"
#include "extension.h"

Extension::Extension(QObject *parent) 
	: QObject(parent)
    , process_(nullptr)
{

}

Extension::~Extension()
{
    if (process_)
    {
        if (process_->state() == QProcess::Running)
            process_->terminate();
        delete process_;
    }
}

bool Extension::query(const QString& prefix)
{
    QStringList arguments;
    if (!process_)
    {
        process_ = new QProcess;
        connect(process_, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
        if (executor_.isEmpty())
        {
            process_->setProgram(executable_);
        }
        else
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
                return false;
            }
            process_->setProgram(*it % exe);
            arguments << executable_;
        }
        process_->setWorkingDirectory(QFileInfo(executable_).absolutePath());
    }
    arguments << prefix;
    process_->setArguments(arguments);
    process_->start();
	return true;
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

const QString &Extension::prefix() const
{
    return prefix_;
}

void Extension::setPrefix(const QString &prefix)
{
    prefix_ = prefix;
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

void Extension::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QByteArray output = process_->readAllStandardOutput();
    // convert json output to action list
}
