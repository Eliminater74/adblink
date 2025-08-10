#include "returncode.h"
#include "logfile.h"
#include <QProcess>
#include <QCoreApplication>

bool returncode(const QString &program, const QStringList &args)
{
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);

    process.start(program, args);

    if (!process.waitForStarted()) {
        logfile("Error: process failed to start: " + process.errorString());
        return false;
    }

    if (!process.waitForFinished()) {
        logfile("Error: process did not finish");
        return false;
    }

    int exitCode = process.exitCode();
    bool success = (exitCode == 0);

    if (!success) {
        QString output = QString::fromUtf8(process.readAll());
        logfile("Process failed with exit code " + QString::number(exitCode));
        logfile("Process output: " + output);
    }

    return success;
}
