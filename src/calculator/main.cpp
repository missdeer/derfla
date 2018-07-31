#include "stdafx.h"
#include "qtsingleapplication.h"
#include <QIcon>
#include "util.h"

bool calculate(const QString& expression)
{
    QTextStream ts(stdout);
    ts.setCodec("UTF-8");

    QJsonDocument d = QJsonDocument::fromJson("[]");
    Q_ASSERT(d.isArray());
    QJsonArray arr = d.array();

    QVariantMap m;
    m.insert("title", expression);
    m.insert("description", QString("Calculate result of expression: %1").arg(expression));
    m.insert("target", expression);
    m.insert("actionType", "copyText");
    m.insert("iconPath", QString(qApp->applicationDirPath() % "/folder.png"));
    arr.append(QJsonObject::fromVariantMap(m));

    d.setArray(arr);
    ts << QString(d.toJson(QJsonDocument::Compact));
    return true;
}

int main(int argc, char *argv[])
{
#if !defined(Q_OS_WIN)
    // increase the number of file that can be opened.
    struct rlimit rl;
    getrlimit(RLIMIT_NOFILE, &rl);

    rl.rlim_cur = qMin(rl.rlim_cur, rl.rlim_max);
    setrlimit(RLIMIT_NOFILE, &rl);
#endif
    SharedTools::QtSingleApplication a("Calculator", argc, argv);

    a.setApplicationName("Calculator");
    a.setApplicationVersion("1.0");
    a.setOrganizationDomain("dfordsoft.com");
    a.setOrganizationName("Derfla");


    if (argc < 3)
    {
        QTextStream ts(stdout);
        ts.setCodec("UTF-8");
        ts << "invalid arguments";
        return 1;
    }

    QString expression;
    for (int i = 2; i < argc; i++)
        expression.append(argv[i]);

    if (!calculate(expression))
        return 1;

    return 0;
}
