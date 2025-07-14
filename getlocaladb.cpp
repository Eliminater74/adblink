#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

QString getlocaladb()
{
    QString databasedir;
#ifdef Q_OS_WIN
    databasedir = QDir::homePath() + "/AppData/Roaming/.jocala/";
#else
    databasedir = QDir::homePath() + "/.jocala/";
#endif
    databasedir = QDir::cleanPath(databasedir);

    // Check if adblink.json exists
    QString jsonPath = databasedir + "/adblink.json";
    if (QFileInfo::exists(jsonPath)) {
        QFile file(jsonPath);
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            file.close();

            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                if (obj.contains("localadb") && !obj["localadb"].toString().isEmpty()) {
                    QString adbPath = QDir::cleanPath(obj["localadb"].toString()) + "/adb";
#ifdef Q_OS_WIN
                    adbPath += ".exe";
#endif



                    if (QFileInfo::exists(adbPath)) {
                        return adbPath;
                    }
                }
            }
        }
    }

    return QString();
}
