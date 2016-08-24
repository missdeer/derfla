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
#endif

#if defined (__cplusplus)

#include <QtCore>
#include <QtGui>
#include <QStyleOption>
#include <QSettings>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QFileDialog>
#include <QTemporaryFile>
#include <QDate>
#include <QDesktopServices>
#include <QDateTime>
#include <QThreadPool>
#include <QCryptographicHash>
#include <QProgressDialog>
#include <QStandardPaths>
#include <QDomDocument>
#include <QTextStream>
#include <QStringBuilder>
#include <QApplication>
#include <QMenu>
#include <QClipboard>
#include <QSplitter>
#include <QHeaderView>
#include <QtConcurrent>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QSharedPointer>
#include <QTabWidget>
#include <QDialog>
#include <QItemSelection>
#include <QAbstractTableModel>
#include <QList>
#include <QFuture>
#include <QMainWindow>
#include <QPoint>
#include <QTableView>
#include <QFileIconProvider>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlDriver>

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QLineEdit>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QHBoxLayout>

#include <boost/scope_exit.hpp>

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
#define globalDefaultFontFamily "Droid Sans"
#endif

#endif

#endif // STDAFX_H
