#include "stdafx.h"

#include <QIcon>

#include "qtsingleapplication.h"
#include "util.h"

void output(QCryptographicHash::Algorithm algo, const QByteArray &data, const QByteArray &res)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson("[]");
    Q_ASSERT(jsonDoc.isArray());
    QJsonArray arr = jsonDoc.array();

    QMap<QCryptographicHash::Algorithm, QString> algoIcon = {
        {QCryptographicHash::Md4, ":/rc/images/md4.png"},
        {QCryptographicHash::Md5, ":/rc/images/md5.png"},
        {QCryptographicHash::Sha1, ":/rc/images/sha1.png"},
        {QCryptographicHash::Sha224, ":/rc/images/sha224.png"},
        {QCryptographicHash::Sha256, ":/rc/images/sha256.png"},
        {QCryptographicHash::Sha384, ":/rc/images/sha384.png"},
        {QCryptographicHash::Sha512, ":/rc/images/sha512.png"},
        {QCryptographicHash::Sha3_224, ":/rc/images/sha3-224.png"},
        {QCryptographicHash::Sha3_256, ":/rc/images/sha3-256.png"},
        {QCryptographicHash::Sha3_384, ":/rc/images/sha3-384.png"},
        {QCryptographicHash::Sha3_512, ":/rc/images/sha3-512.png"},
        {QCryptographicHash::Keccak_224, ":/rc/images/keccak224.png"},
        {QCryptographicHash::Keccak_256, ":/rc/images/keccak256.png"},
        {QCryptographicHash::Keccak_384, ":/rc/images/keccak384.png"},
        {QCryptographicHash::Keccak_512, ":/rc/images/keccak512.png"},
    };

    QMap<QCryptographicHash::Algorithm, QString> algoName = {
        {QCryptographicHash::Md4, "MD4"},
        {QCryptographicHash::Md5, "MD5"},
        {QCryptographicHash::Sha1, "SHA1"},
        {QCryptographicHash::Sha224, "SHA224"},
        {QCryptographicHash::Sha256, "SHA256"},
        {QCryptographicHash::Sha384, "SHA384"},
        {QCryptographicHash::Sha512, "SHA512"},
        {QCryptographicHash::Sha3_224, "SHA3-224"},
        {QCryptographicHash::Sha3_256, "SHA3-256"},
        {QCryptographicHash::Sha3_384, "SHA3-384"},
        {QCryptographicHash::Sha3_512, "SHA3-512"},
        {QCryptographicHash::Keccak_224, "KECCAK224"},
        {QCryptographicHash::Keccak_256, "KECCAK256"},
        {QCryptographicHash::Keccak_384, "KECCAK384"},
        {QCryptographicHash::Keccak_512, "KECCAK512"},
    };
    QVariantMap varMap;
    varMap.insert("title", res);
    if (QFile::exists(QString(data)))
    {
        QFileInfo fileInfo((QString(data)));
        varMap.insert("description", QObject::tr("%1 result of file %2").arg(algoName[algo]).arg(fileInfo.fileName()));
    }
    else
    {
        varMap.insert("description", QObject::tr("%1 result of string \"%2\"").arg(algoName[algo]).arg(QString(data.left(64))));
    }
    varMap.insert("target", res);
    varMap.insert("actionType", "copyText");
    QFile icon(algoIcon[algo]);
    if (icon.open(QIODevice::ReadOnly))
    {
        auto bytes = icon.readAll();
        icon.close();
        varMap.insert("iconData", bytes.toBase64());
    }
    arr.append(QJsonObject::fromVariantMap(varMap));

    jsonDoc.setArray(arr);
    QTextStream stdoutTs(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stdoutTs.setCodec("UTF-8");
#else
    stdoutTs.setEncoding(QStringConverter::Utf8);
#endif
    stdoutTs << QString(jsonDoc.toJson(QJsonDocument::Compact));
}

bool hashString(QCryptographicHash::Algorithm algo, const QByteArray &data)
{
    auto res = QCryptographicHash::hash(data, algo);
    output(algo, data, res.toHex());
    return true;
}

bool hashFile(QCryptographicHash::Algorithm algo, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QCryptographicHash hash(algo);
    const qint64       mapSize = 1024 * 1024;
    qint64             size    = file.size();
    qint64             offset  = 0;
    while (offset < size)
    {
        qint64 length = qMin(mapSize, size - offset);
        auto  *pos    = file.map(offset, length);
        hash.addData((const char *)pos, length);
        file.unmap(pos);
        offset += length;
    }

    file.close();
    auto res = hash.result();
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
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("HashDigest");
    QCoreApplication::setApplicationVersion("1.0");
    QCoreApplication::setOrganizationDomain("ismisv.com");
    QCoreApplication::setOrganizationName("Derfla");

    QTextStream stdoutTs(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stdoutTs.setCodec("UTF-8");
#else
    stdoutTs.setEncoding(QStringConverter::Utf8);
#endif
    if (argc != 3 && argc != 4)
    {
        stdoutTs << "invalid arguments";
        return 1;
    }

    auto        uiLanguages = QLocale().uiLanguages();
    auto       &locale      = uiLanguages[0];
    QTranslator translator;
    QTranslator qtTranslator;

    // main application and dynamic linked library locale
#if defined(Q_OS_MAC)
    QString rootDirPath   = QCoreApplication::applicationDirPath() + "/../../Resources/translations";
    QString localeDirPath = QCoreApplication::applicationDirPath() + "/translations";
#else
    QString rootDirPath   = QCoreApplication::applicationDirPath() + "/../../translations";
    QString localeDirPath = QCoreApplication::applicationDirPath() + "/translations";
#endif

    if (!translator.load("hashdigest_" + locale, localeDirPath))
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " failed";
    }
    else
    {
        qDebug() << "loading " << locale << " from " << localeDirPath << " success";
        if (!QCoreApplication::installTranslator(&translator))
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
        if (!QCoreApplication::installTranslator(&qtTranslator))
        {
            qDebug() << "installing qt translator failed ";
        }
    }

    QMap<QString, QCryptographicHash::Algorithm> algo = {
        {"md4", QCryptographicHash::Md4},
        {"md5", QCryptographicHash::Md5},
        {"sha1", QCryptographicHash::Sha1},
        {"sha224", QCryptographicHash::Sha224},
        {"sha256", QCryptographicHash::Sha256},
        {"sha384", QCryptographicHash::Sha384},
        {"sha512", QCryptographicHash::Sha512},
        {"sha3-224", QCryptographicHash::Sha3_224},
        {"sha3-256", QCryptographicHash::Sha3_256},
        {"sha3-384", QCryptographicHash::Sha3_384},
        {"sha3-512", QCryptographicHash::Sha3_512},
        {"keccak224", QCryptographicHash::Keccak_224},
        {"keccak256", QCryptographicHash::Keccak_256},
        {"keccak384", QCryptographicHash::Keccak_384},
        {"keccak512", QCryptographicHash::Keccak_512},
    };

    auto args = SharedTools::QtSingleApplication::arguments();

    auto iter = algo.find(QString(args[1]));
    if (algo.end() == iter)
    {
        stdoutTs << "unsupported algorithm";
        return 2;
    }

    QString option;
    QString data;

    if (argc == 3)
    {
        data   = args[2];
        
        if (QFile::exists(data))
        {
            option = "f";
        }
    }

    if (argc == 4)
    {
        option = args[2];
        data   = args[3];
        if (option.toLower() != "f" || !QFile::exists(data))
        {
            option.clear();
        }
    }

    bool success = true;
    if (option.toLower() == "f")
    {
        success = hashFile(iter.value(), data);
    }
    else
    {
        success = hashString(iter.value(), data.toUtf8());
    }

    return success ? 0 : 3;
}
