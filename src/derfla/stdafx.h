#ifndef STDAFX_H
#define STDAFX_H


#if defined(_WIN32)
#define NOMINMAX
#include <Windows.h>
#include <strsafe.h>
#include <PSapi.h>
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
#    include <QApplication>
#    include <QCheckBox>
#    include <QComboBox>
#    include <QDialog>
#    include <QFileDialog>
#    include <QFileIconProvider>
#    include <QHBoxLayout>
#    include <QHeaderView>
#    include <QInputDialog>
#    include <QKeySequenceEdit>
#    include <QLabel>
#    include <QLineEdit>
#    include <QListView>
#    include <QMainWindow>
#    include <QMenu>
#    include <QMessageBox>
#    include <QProgressDialog>
#    include <QSlider>
#    include <QSpinBox>
#    include <QSplitter>
#    include <QStyleOption>
#    include <QTabWidget>
#    include <QTableView>
#    include <QVBoxLayout>

#    if defined(Q_OS_WIN)
#        define globalDefaultFontFamily "Microsoft YaHei"
#    elif defined(Q_OS_MAC)
#        define globalDefaultFontFamily "PingFang CS"
#    else
#        define globalDefaultFontFamily "DejaVu Sans"
#    endif

#endif

#endif // STDAFX_H
