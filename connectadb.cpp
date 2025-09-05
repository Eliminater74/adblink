#include "connectadb.h"
#include "logfile.h"

#include <QProcess>
#include <QFileInfo>
#include <QTimer>
#include <QEventLoop>

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <signal.h>
#include <unistd.h>
#endif

QString connectadb(const QString &adbPath, const QStringList &arguments)
{
    logfile("Starting connection process");

    logfile("Executable: " + adbPath);
    logfile("Arguments: " + arguments.join(" "));

    QFileInfo adbInfo(adbPath);
    //logfile("adb exists: " + QString::number(adbInfo.exists()));
    //logfile("adb is executable: " + QString::number(adbInfo.isExecutable()));

    QProcess run_command;
    run_command.setProcessChannelMode(QProcess::MergedChannels);
    run_command.start(adbPath, arguments);

    if (!run_command.waitForStarted()) {
        logfile("Failed to start the process.");
        logfile("PATH: " + qgetenv("PATH"));
        logfile("QProcess error: " + run_command.errorString());
        return QString();
    }

    //logfile("Attempting to connect ...");

    bool forceTerminated = false;
    QTimer timer;
    timer.setSingleShot(true);
    timer.start(5000); // 5-second timeout

    QEventLoop loop;

    QObject::connect(&timer, &QTimer::timeout, [&]() {
#ifdef Q_OS_WIN
        DWORD processId = run_command.processId();
        HANDLE processHandle = ::OpenProcess(PROCESS_TERMINATE, FALSE, processId);
        if (processHandle) {
            ::TerminateProcess(processHandle, 0);
            ::CloseHandle(processHandle);
            logfile("Connection process terminated.");
        } else {
            logfile("Failed to open process handle for termination.");
        }
#elif defined(Q_OS_UNIX)
            ::kill(run_command.processId(), SIGKILL);
            logfile("IP not responding. Process killed.");
#endif
        forceTerminated = true;
        loop.quit();
    });

    QObject::connect(&run_command, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     [&](int, QProcess::ExitStatus) {
                         if (!forceTerminated) {
                            // logfile("Process exited normally during connect.");
                         }
                         loop.quit();
                     });

    loop.exec();

    if (run_command.state() != QProcess::NotRunning) {
        run_command.terminate();
        run_command.waitForFinished();
    }

    QString output = run_command.readAll();
 //   logfile("Process output:");
  //  logfile(output.trimmed());

    return output;
}
