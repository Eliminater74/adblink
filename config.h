#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QCoreApplication>

// Global adb path, lazily initialized
#define adb (adb_internal())

inline const QString& adb_internal() {
    static const QString adbPath = QCoreApplication::applicationDirPath() + "/adbfiles/adb";
    return adbPath;
}

#endif // CONFIG_H
