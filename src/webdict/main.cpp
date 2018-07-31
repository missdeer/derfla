#include "stdafx.h"
#include "qtsingleapplication.h"
#include <QIcon>
#include "youdao.h"
#include "bing.h"
#include "util.h"

int main(int argc, char *argv[])
{
#if !defined(Q_OS_WIN)
    // increase the number of file that can be opened.
    struct rlimit rl;
    getrlimit(RLIMIT_NOFILE, &rl);

    rl.rlim_cur = qMin(rl.rlim_cur, rl.rlim_max);
    setrlimit(RLIMIT_NOFILE, &rl);
#endif
    SharedTools::QtSingleApplication a("WebDictionary", argc, argv);

    a.setApplicationName("WebDictionary");
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

    QString cmd(argv[1]);
    QStringList input;

#if defined(Q_OS_WIN)
    int nArgs = 0;

    LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    for (int i = 2; i < argc; i++) {
        input.append(QString::fromWCharArray(szArglist[i]));
    }
#else
    for (int i = 2; i < argc; i++) {
        input.append(QString(argv[i]));
    }
#endif
    if (cmd == "dict" || cmd == "yd" || cmd == "youdao") {
        Youdao yd;
        yd.query(input.join(' '));
        return a.exec();
    }

    if (cmd == "dict" || cmd == "bing"){
        Bing bing;
        bing.query(input.join(' '));
        return a.exec();
    }

    return -1;
}
