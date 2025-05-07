// adbutils.cpp
#include "adbutils.h"
#include <QCoreApplication>
#include <QDir>

QString getadbpath()
{
    QString path = QCoreApplication::applicationDirPath() + "/adbfiles/adb";
#ifdef Q_OS_WIN
    path += ".exe"; // Windows-specific
#endif
    return QDir::cleanPath(path);
}
