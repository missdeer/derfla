#ifndef STDAFX_H
#define STDAFX_H

#if defined(_WIN32)
#    define NOMINMAX
#    include <Windows.h>

#    include <PSapi.h>
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
#        define globalDefaultFontFamily "PingFang SC"
#    else
#        define globalDefaultFontFamily "DejaVu Sans"
#    endif

#    ifndef Q_MOC_RUN
#        if defined(emit)
#            undef emit
#            include <oneapi/tbb.h>
#            define emit // restore the macro definition of "emit", as it was defined in gtmetamacros.h
#        else
#            include <oneapi/tbb.h>
#        endif // defined(emit)
#    endif     // Q_MOC_RUN
#endif

#endif // STDAFX_H
