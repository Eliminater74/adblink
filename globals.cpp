#include "globals.h"
#include <QCoreApplication>
#include <QDir>

QString adbfiles;
QString aapt;
QString busybox;
QString apphome;
QString scrcpydir;
QString xmldir;
QString splashdir;
QString tempdir;
QString databasedir;
QString logfiledir;
QString scriptdir;


void initGlobals()
{
    apphome   = QCoreApplication::applicationDirPath();
    adbfiles  = apphome + "/adbfiles/";
    aapt      = adbfiles + "aapt";
    busybox   = adbfiles + "busybox";
    scrcpydir = adbfiles + "scrcpy/";
    xmldir    = adbfiles + "remotes/";
    splashdir = adbfiles + "splash/";
    tempdir   = "/data/local/tmp/";
    databasedir = QDir::homePath() + "/.jocala/";
    logfiledir  = QDir::homePath() + "/.jocala/";
    scriptdir  = QDir::homePath() + "/.jocala/scripts";



}
