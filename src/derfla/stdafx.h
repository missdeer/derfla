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

// QtGui
#include <QtGui>
#include <QDesktopServices>
#include <QClipboard>
#include <QDropEvent>
#include <QDragEnterEvent>

// Qt Widgets
#include <QApplication>
#include <QStyleOption>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>
#include <QMenu>
#include <QSplitter>
#include <QHeaderView>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QTabWidget>
#include <QDialog>
#include <QMainWindow>
#include <QTableView>
#include <QFileIconProvider>
#include <QLineEdit>
#include <QHBoxLayout>

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

#include "util.h"

#define check_expiration do { \
    QDate d =  QLocale(QLocale::C).toDate(QString(__DATE__).simplified(), QLatin1String("MMM d yyyy")); \
    if (d.daysTo(QDate::currentDate()) > 30) { \
        QMessageBox::critical(NULL, tr("Expired"), tr("This application has been expired, please visit http://www.getderfla.com for a new build."), QMessageBox::Ok ); \
        qApp->quit(); \
    } \
}while(0)

#if defined(Q_OS_WIN)
#define globalDefaultFontFamily "Microsoft YaHei"
#elif defined(Q_OS_MAC)
#define globalDefaultFontFamily "PingFang CS"
#else
#define globalDefaultFontFamily "DejaVu Sans"
#endif

#endif

#endif // STDAFX_H
