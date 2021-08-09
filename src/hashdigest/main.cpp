#include "stdafx.h"
#include "qtsingleapplication.h"
#include <QIcon>
#include "util.h"

void output(QCryptographicHash::Algorithm algo, const QByteArray& data, const QByteArray& res)
{
    QJsonDocument d = QJsonDocument::fromJson("[]");
    Q_ASSERT(d.isArray());
    QJsonArray arr = d.array();


    QMap<QCryptographicHash::Algorithm, QString> algoIcon = {
        { QCryptographicHash::Md4       , ":/rc/images/md4.png"        },
        { QCryptographicHash::Md5       , ":/rc/images/md5.png"        },
        { QCryptographicHash::Sha1      , ":/rc/images/sha1.png"       },
        { QCryptographicHash::Sha224    , ":/rc/images/sha224.png"     },
        { QCryptographicHash::Sha256    , ":/rc/images/sha256.png"     },
        { QCryptographicHash::Sha384    , ":/rc/images/sha384.png"     },
        { QCryptographicHash::Sha512    , ":/rc/images/sha512.png"     },
        { QCryptographicHash::Sha3_224  , ":/rc/images/sha3-224.png"   },
        { QCryptographicHash::Sha3_256  , ":/rc/images/sha3-256.png"   },
        { QCryptographicHash::Sha3_384  , ":/rc/images/sha3-384.png"   },
        { QCryptographicHash::Sha3_512  , ":/rc/images/sha3-512.png"   },
        { QCryptographicHash::Keccak_224, ":/rc/images/keccak224.png"  },
        { QCryptographicHash::Keccak_256, ":/rc/images/keccak256.png"  },
        { QCryptographicHash::Keccak_384, ":/rc/images/keccak384.png"  },
        { QCryptographicHash::Keccak_512, ":/rc/images/keccak512.png"  },
    };

    QMap<QCryptographicHash::Algorithm, QString> algoName = {
        { QCryptographicHash::Md4       , "MD4" },
        { QCryptographicHash::Md5       , "MD5" },
        { QCryptographicHash::Sha1      , "SHA1" },
        { QCryptographicHash::Sha224    , "SHA224" },
        { QCryptographicHash::Sha256    , "SHA256" },
        { QCryptographicHash::Sha384    , "SHA384" },
        { QCryptographicHash::Sha512    , "SHA512" },
        { QCryptographicHash::Sha3_224  , "SHA3-224" },
        { QCryptographicHash::Sha3_256  , "SHA3-256" },
        { QCryptographicHash::Sha3_384  , "SHA3-384" },
        { QCryptographicHash::Sha3_512  , "SHA3-512" },
        { QCryptographicHash::Keccak_224, "KECCAK224" },
        { QCryptographicHash::Keccak_256, "KECCAK256" },
        { QCryptographicHash::Keccak_384, "KECCAK384" },
        { QCryptographicHash::Keccak_512, "KECCAK512" },
    };
    QVariantMap m;
    m.insert("title", res );
    if (QFile::exists(QString(data)))
    {
        QFileInfo f((QString(data)));
        m.insert("description", QString(QObject::tr("%1 result of file %2")).arg(algoName[algo]).arg(f.fileName()));
    }
    else
    {
        m.insert("description", QString(QObject::tr("%1 result of string \"%2\"")).arg(algoName[algo]).arg(QString(data.left(64))));
    }
    m.insert("target", res);
    m.insert("actionType", "copyText");
    QFile icon(algoIcon[algo]);
    if (icon.open(QIODevice::ReadOnly))
    {
        auto bytes = icon.readAll();
        icon.close();
        m.insert("iconData", bytes.toBase64());
    }
    arr.append(QJsonObject::fromVariantMap(m));

    d.setArray(arr);
    QTextStream ts(stdout);
    ts.setCodec("UTF-8");
    ts << QString(d.toJson(QJsonDocument::Compact));
}

bool hashString(QCryptographicHash::Algorithm algo, const QByteArray& data)
{
    auto res = QCryptographicHash::hash(data, algo);
    output(algo, data, res.toHex());
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
    output(algo, filePath.toUtf8(), res.toHex());
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
    a.setOrganizationDomain("ismisv.com");
    a.setOrganizationName("Derfla");

    QTextStream ts(stdout);
    ts.setCodec("UTF-8");
    if (argc != 3 && argc != 4)
    {
        ts << "invalid arguments";
        return 1;
    }

    auto        uiLanguages = QLocale().uiLanguages();
    auto &      locale      = uiLanguages[0];
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

    if (!translator.load("hashdigest_" + locale, localeDirPath))
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

#if defined(Q_OS_WIN)
    int nArgs = 0;

    LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
#endif

    auto it = algo.find(QString(argv[1]));
    if (algo.end() == it)
    {
        ts << "unsupported algorithm";
        return 2;
    }

    QString option, data;

    if (argc == 3)
    {

#if defined(Q_OS_WIN)
        data = QString::fromWCharArray(szArglist[2]);
        LocalFree(szArglist);
#else
        data = argv[2];
#endif
        if (QFile::exists(data))
            option = "f";
    }

    if (argc == 4)
    {
#if defined(Q_OS_WIN)
        option = QString::fromWCharArray(szArglist[2]);
        data = QString::fromWCharArray(szArglist[3]);
        LocalFree(szArglist);
#else
        option = argv[2];
        data = argv[3];
#endif
        if (option.toLower() != "f" || !QFile::exists(data))
            option.clear();
    }

    bool success = true;
    if (option.toLower() == "f")
        success = hashFile(it.value(), data);
    else
        success = hashString(it.value(), data.toUtf8());

    return success ? 0 : 3;
}
