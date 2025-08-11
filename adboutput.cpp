// adboutput.cpp
#include "adboutput.h"
#include "logfile.h"
#include <QProcess>
#include <QDebug>
#include <QCoreApplication>

QString getadbOutput2(const QString &adbPath, const QStringList &args)
{



    QProcess run_command;
    run_command.setProcessChannelMode(QProcess::MergedChannels);

    run_command.start(adbPath, args);

    if (!run_command.waitForStarted()) {
        QString err = "Error: process failed to start: " + run_command.errorString();
        logfile(err);
        return err;
    }

    if (!run_command.waitForFinished()) {
        QString err = "Error: process did not finish";
        logfile(err);
        return err;
    }

    QString output = QString::fromUtf8(run_command.readAll());
    logfile("ADB output: " + output);

    return output;
}
