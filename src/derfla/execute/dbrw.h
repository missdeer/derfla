#ifndef DBRW_H
#define DBRW_H

#include <QObject>

class DBRW : public QObject
{
    Q_OBJECT
public:
    static DBRW* instance();
    static void destroy();
signals:

public slots:

private:
    static DBRW *instance_;
    DBRW();
    ~DBRW();
};

#endif // DBRW_H