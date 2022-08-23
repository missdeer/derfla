#ifndef STDAFX_H
#define STDAFX_H

#if defined(_WIN32)
#    define NOMINMAX
#    include <Everything.h>
#    include <PSapi.h>
#    include <Shellapi.h>
#    include <Shlobj.h>
#    include <Windows.h>
#    include <Winnt.h>
#    include <everything_ipc.h>
#    include <shlguid.h>
#    include <shobjidl.h>
#    include <strsafe.h>
#elif defined(Q_OS_MAC)
#else
#    include <sys/resource.h>
#endif

#if defined(__cplusplus)

// QtCore
#    include <QAbstractTableModel>
#    include <QCommandLineOption>
#    include <QCommandLineParser>
#    include <QCryptographicHash>
#    include <QDate>
#    include <QDateTime>
#    include <QDir>
#    include <QFile>
#    include <QFileInfo>
#    include <QFuture>
#    include <QFutureWatcher>
#    include <QItemSelection>
#    include <QList>
#    include <QPoint>
#    include <QProcess>
#    include <QSettings>
#    include <QSharedPointer>
#    include <QStandardPaths>
#    include <QStringBuilder>
#    include <QTemporaryFile>
#    include <QTextStream>
#    include <QThreadPool>
#    include <QUrl>
#    include <QtCore>

// Qt Xml
#    include <QDomDocument>

// Qt Concurrent
#    include <QtConcurrent>

// Qt Sql
#    include <QSqlDatabase>
#    include <QSqlDriver>
#    include <QSqlError>
#    include <QSqlQuery>
#    include <QSqlRecord>
#endif

#endif // STDAFX_H
