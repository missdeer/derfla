#ifndef STDAFX_H
#define STDAFX_H


#if defined(_WIN32)
#define NOMINMAX
#include <Windows.h>
#include <Winnt.h>
#include <Shellapi.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <Shlobj.h>
#include <strsafe.h>
#include <PSapi.h>
#include <Everything.h>
#include <everything_ipc.h>
#elif defined(Q_OS_MAC)
#else
#include <sys/resource.h>
#endif

#if defined (__cplusplus)

// QtCore
#include <QtCore>
#include <QFuture>
#include <QFutureWatcher>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QUrl>
#include <QTemporaryFile>
#include <QDate>
#include <QDateTime>
#include <QThreadPool>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QTextStream>
#include <QSharedPointer>
#include <QItemSelection>
#include <QAbstractTableModel>
#include <QList>
#include <QPoint>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QStringBuilder>
#include <QProcess>

// Qt Xml
#include <QDomDocument>

// Qt Concurrent
#include <QtConcurrent>

// Qt Sql
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlDriver>

// Boost
#include <boost/scope_exit.hpp>

#endif

#endif // STDAFX_H
