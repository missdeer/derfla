#include "stdafx.h"
#include "qtsingleapplication.h"
#include <QIcon>
#include "core/constants.h"
#include "core/evaluator.h"
#include "core/functions.h"
#include "core/numberformatter.h"
#include "core/settings.h"
#include "core/session.h"
#include "core/variable.h"
#include "core/sessionhistory.h"
#include "core/userfunction.h"
#include "math/floatconfig.h"
#include "util.h"

bool calculate(const QString& expression)
{
    QTextStream ts(stdout);
    ts.setCodec("UTF-8");

    QJsonDocument d = QJsonDocument::fromJson("[]");
    Q_ASSERT(d.isArray());
    QJsonArray arr = d.array();

    Session* m_session = new Session();
    Evaluator*    m_evaluator = Evaluator::instance();
    m_evaluator->setSession(m_session);
    m_evaluator->initializeBuiltInVariables();

    QString expr = m_evaluator->autoFix(expression);

    if (expr.isEmpty())
        return false;

    m_evaluator->setExpression(expr);
    Quantity result = m_evaluator->evalUpdateAns();

    if (!m_evaluator->error().isEmpty()) {
        ts << m_evaluator->error();
        return false;
    }

    if (m_evaluator->isUserFunctionAssign()) {
        result = CMath::nan();
    } else if (result.isNan())
        return false;

    QString strToCopy(NumberFormatter::format(result));
    strToCopy.replace(QChar(0x2212), QChar('-'));

    QVariantMap m;
    m.insert("title", QString(QObject::tr("Result: %1")).arg(strToCopy));
    m.insert("description", QString(QObject::tr("Expr: %1")).arg(expression));
    m.insert("target", strToCopy);
    m.insert("actionType", "copyText");
    QFile icon(":/calculator.png");
    if (icon.open(QIODevice::ReadOnly))
    {
        auto bytes = icon.readAll();
        icon.close();
        m.insert("iconData", bytes.toBase64());
    }
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
    a.setOrganizationDomain("minidump.info");
    a.setOrganizationName("Derfla");


    if (argc < 3)
    {
        QTextStream ts(stdout);
        ts.setCodec("UTF-8");
        ts << "invalid arguments";
        return 1;
    }

    QString locale = QLocale().uiLanguages()[0];
    QTranslator translator;
    QTranslator qtTranslator;

    // main application and dynamic linked library locale
#if defined(Q_OS_MAC)
    QString rootDirPath = QApplication::applicationDirPath() + "/../../Resources/translations";
    QString localeDirPath = QApplication::applicationDirPath() + "/translations";
#else
    QString rootDirPath = QApplication::applicationDirPath() + "/../../translations";
    QString localeDirPath = QApplication::applicationDirPath() + "/translations";
#endif

    if (!translator.load("calculate_" + locale, localeDirPath))
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " failed";
    }
    else
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " success";
        if (!a.installTranslator(&translator))
        {
            qDebug() << "installing translator failed ";
        }
    }

    // qt locale
    if (!qtTranslator.load("qt_" + locale, rootDirPath))
    {
        qDebug() << "loading " << locale << " from " << rootDirPath << " failed";
    }
    else
    {
        qDebug() << "loading " << locale << " from " << rootDirPath << " success";
        if (!a.installTranslator(&qtTranslator))
        {
            qDebug() << "installing qt translator failed ";
        }
    }

    QString expression;
    for (int i = 2; i < argc; i++)
        expression.append(argv[i]);

    if (!calculate(expression))
        return 1;

    return 0;
}
