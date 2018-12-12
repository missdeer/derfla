#include "qrcodedialog.h"
#include <QApplication>

bool commandLineUrlArgument()
{
    const QStringList args = QCoreApplication::arguments();
    for (const QString &arg : args.mid(1)) {
        if (arg == "--alipay")
            return  true;
    }
    return false;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QrcodeDialog w(commandLineUrlArgument());
    w.show();

    return a.exec();
}
