#include "stdafx.h"
#include "qtsingleapplication.h"
#include <QIcon>
#include "util.h"

bool hashString(QCryptographicHash::Algorithm algo, const QByteArray& data)
{
    auto res = QCryptographicHash::hash(data, algo);

    return true;
}

bool hashFile(QCryptographicHash::Algorithm algo, const QString& filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QCryptographicHash h(algo);
    const qint64 mapSize = 1024 * 1024;
    qint64 size = f.size();
    qint64 offset = 0;
    while (offset < size)
    {
        qint64 length = qMin(mapSize, size - offset);
        auto pos = f.map(offset, length);
        h.addData((const char *)pos, length);
        f.unmap(pos);
        offset += length;
    }

    f.close();
    auto res = h.result();

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
    SharedTools::QtSingleApplication a("HashDigest", argc, argv);

    a.setApplicationName("HashDigest");
    a.setApplicationVersion("1.0");
    a.setOrganizationDomain("dfordsoft.com");
    a.setOrganizationName("Derfla");

    QTextStream ts(stdout);
    ts.setCodec("UTF-8");
    if (argc != 3 && argc != 4)
    {
        ts << "invalid arguments";
        return 1;
    }

    QMap<QString, QCryptographicHash::Algorithm> algo = {
        {"md4"       , QCryptographicHash::Md4        },
        {"md5"       , QCryptographicHash::Md5        },
        {"sha1"      , QCryptographicHash::Sha1       },
        {"sha224"    , QCryptographicHash::Sha224     },
        {"sha256"    , QCryptographicHash::Sha256     },
        {"sha384"    , QCryptographicHash::Sha384     },
        {"sha512"    , QCryptographicHash::Sha512     },
        {"sha3-224"  , QCryptographicHash::Sha3_224   },
        {"sha3-256"  , QCryptographicHash::Sha3_256   },
        {"sha3-384"  , QCryptographicHash::Sha3_384   },
        {"sha3-512"  , QCryptographicHash::Sha3_512   },
        {"keccak224" , QCryptographicHash::Keccak_224 },
        {"keccak256" , QCryptographicHash::Keccak_256 },
        {"keccak384" , QCryptographicHash::Keccak_384 },
        {"keccak512" , QCryptographicHash::Keccak_512 },
    };

    auto it = algo.find(QString(argv[1]));
    if (algo.end() == it)
    {
        ts << "unsupported algorithm";
        return 2;
    }

    if (argc == 3)
    {
        QString data(argv[2]);
        if (QFile::exists(data))
        {
            if (!hashFile(it.value() , data))
            {
                return 3;
            }
        }
        else
        {
            if (!hashString(it.value(), data.toUtf8()))
            {
                return 3;
            }
        }
    }

    if (argc == 4)
    {
        QString option(argv[2]);
        QString data(argv[3]);
        if (option.toLower() == "f" && QFile::exists(data))
        {
            if (!hashFile(it.value() , data))
            {
                return 3;
            }
        }
        else
        {
            if (!hashString(it.value(), data.toUtf8()))
            {
                return 3;
            }
        }
    }

    return 0;
}
