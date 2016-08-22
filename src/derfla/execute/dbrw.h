#ifndef DBRW_H
#define DBRW_H

#include <QObject>

class DBRW : public QObject
{
    Q_OBJECT
public:
    static DBRW* instance();
    static void destroy();
    bool insertLFS(const QByteArray& icon, const QString& title, const QString& description, const QString& target, const QString& arguments, const QString workingDirectory, quint64 timestamp, quint64 lastModified, const QString& type);
signals:

public slots:

private:
    static DBRW *instance_;
    QString dbPath_;
    QSqlDatabase db_;
    DBRW();
    ~DBRW();
    bool createDatabase();
    bool openDatabase();
};

#endif // DBRW_H
