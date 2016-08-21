#include "stdafx.h"
#include "localfsscanner.h"

LocalFSScanner::LocalFSScanner(QObject *parent) : QObject(parent)
{
    connect(this, &LocalFSScanner::finished, [this] {QTimer::singleShot(10 *60* 1000, this, &LocalFSScanner::scan);});

    workerThread.start(QThread::IdlePriority);
}

LocalFSScanner::~LocalFSScanner()
{
    if (workerThread.isRunning())
    {
        workerThread.quit();
        workerThread.wait();
    }
}

void LocalFSScanner::start()
{
    this->moveToThread(&workerThread);
    QTimer::singleShot(1000, this, &LocalFSScanner::scan);
}

void LocalFSScanner::scan()
{
    qDebug() << "LocalFSScanner::scan" << QThread::currentThreadId();
    emit finished();
}
