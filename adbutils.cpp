
#include "adbutils.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include "getlocaladb.h"

QString getadbpath()
{
    QString databasedir;
    QString adbPath;

#ifdef Q_OS_WIN
    databasedir = QDir::homePath() + "/AppData/Roaming/.jocala/";
#else
    databasedir = QDir::homePath() + "/.jocala/";
#endif

  adbPath = QCoreApplication::applicationDirPath() + "/adbfiles/adb";

#ifdef Q_OS_WIN
    adbPath += ".exe"; // Windows-specific
#endif
    adbPath = QDir::cleanPath(adbPath);


    if (!getlocaladb().isEmpty())
        adbPath=getlocaladb();

    return adbPath;
}
