#ifndef DBRW_H
#define DBRW_H

#include <QObject>

class DBRW : public QObject
{
    Q_OBJECT
public:
    static DBRW* instance();
    static void destroy();

    bool firstLaunch() const { return firstLaunch_;  }
    bool getLFSActions(DerflaActionList& dal, const QString &keyword, int countRequired = 50);
    bool removeOldRecords(qint64 timestamp);
    bool insertLFS(const QByteArray& icon, const QString& title, const QString& description, const QString& target, const QString& arguments, const QString workingDirectory, qint64 timestamp, qint64 lastModified, const QString& type);
signals:

public slots:

private:
    static DBRW *instance_;
    bool firstLaunch_ = false;
    QString dbPath_;
    QSqlDatabase db_;
    DBRW();
    ~DBRW();
    bool createDatabase();
    bool openDatabase();
    bool queryActions(DerflaActionList& dal, const QString &keyword, int countRequired, QSqlQuery& q);
};

#endif // DBRW_H
