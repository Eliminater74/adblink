#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>

// Global variable declarations
extern QString adbfiles;
extern QString aapt;
extern QString busybox;
extern QString apphome;
extern QString scrcpydir;
extern QString xmldir;
extern QString splashdir;
extern QString tempdir;
extern QString databasedir;
extern QString logfiledir;
extern QString scriptdir;

void initGlobals();
#endif // GLOBALS_H
