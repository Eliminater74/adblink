
#include "adbutils.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

QString getadbpath()
{
    QString databasedir;
#ifdef Q_OS_WIN
    databasedir = QDir::homePath() + "/AppData/Roaming/.jocala/";
#else
    databasedir = QDir::homePath() + "/.jocala/";
#endif

    QString adbPath = QCoreApplication::applicationDirPath() + "/adbfiles/adb";
#ifdef Q_OS_WIN
    adbPath += ".exe"; // Windows-specific
#endif
    adbPath = QDir::cleanPath(adbPath);

    // Check if adblink.json exists
    if (QFileInfo::exists(databasedir + "/adblink.json")) {
        QFile file(databasedir + "/adblink.json");
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            file.close();

            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                if (obj.contains("localadb") && !obj["localadb"].toString().isEmpty()) {
                    QString localAdbPath = QDir::cleanPath(obj["localadb"].toString());
                    // Append /adb or /adb.exe to localAdbPath
                    localAdbPath += QDir::separator() + QString("adb");
#ifdef Q_OS_WIN
                    localAdbPath += ".exe";
#endif
                    if (QFileInfo::exists(localAdbPath)) {
                        adbPath = localAdbPath;
                    }
                }
            }
        }
    }

    return adbPath;
}
