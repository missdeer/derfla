#ifndef STDAFX_H
#define STDAFX_H

#if defined(_WIN32)
#    define NOMINMAX
#    include <PSapi.h>
#    include <Windows.h>
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

// QtGui
#    include <QClipboard>
#    include <QDesktopServices>
#    include <QDragEnterEvent>
#    include <QDropEvent>
#    include <QtGui>

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
