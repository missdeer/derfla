#ifndef STDAFX_H
#define STDAFX_H

#if defined(_WIN32)
#    define NOMINMAX
#    include <Windows.h>

#    include <PSapi.h>
#    include <Shellapi.h>
#    include <Shlobj.h>
#    include <Winnt.h>
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
#endif

#endif // STDAFX_H
