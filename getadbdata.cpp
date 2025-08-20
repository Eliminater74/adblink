#include "getadbdata.h"
#include "adbutils.h"
#include "logfile.h"
#include <QDebug>
#include <QProcess>
#include <QCoreApplication>

struct adbReturn {
    int returncode;
    QString returnText;
};


QString getadbOutput(const QString &cstring)
{

  QString program=getadbpath();
  QStringList args = QProcess::splitCommand(cstring);

  args.removeFirst();


  QProcess run_command;


  run_command.setProcessChannelMode(QProcess::MergedChannels);

 // run_command.start(cstring);

 run_command.start(program,args);

  run_command.waitForStarted();


  while(run_command.state() != QProcess::NotRunning)
      qApp->processEvents();

  QString command=run_command.readAll();


 // bool success = (run_command.exitCode() == 0);

  return command;

}
