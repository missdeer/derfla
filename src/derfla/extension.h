#ifndef EXTENSION_H
#define EXTENSION_H

#include <QObject>
#include "derflaaction.h"

class Extension : public QObject
{
    Q_OBJECT
public:
    explicit Extension(QObject *parent = nullptr);

	bool load(const QString& configuration);
	bool query(const QString& prefix);
	bool run(DerflaAction* action);
signals:
	void queried(DerflaActionList &);
public slots:

private:
	DerflaActionList derflaActions_;
};

typedef QSharedPointer<Extension> ExtensionPtr;

#endif // Extension_H
