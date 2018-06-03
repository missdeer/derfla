#ifndef EXTENSION_H
#define EXTENSION_H

#include <QObject>

class Extension : public QObject
{
    Q_OBJECT
public:
    explicit Extension(QObject *parent = nullptr);

signals:

public slots:
};

#endif // Extension_H
