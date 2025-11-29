#include "getdevices.h"
#include "getadbdata.h"
#include "logfile.h"
#include "adbutils.h"
#include "getlocaladb.h"
#include <QDebug>

getdevices::getdevices(QObject *parent)
    : QObject{parent}
{

    QString cstring;
    QString command;
    QStringList mstringlist;
    QStringList dstringlist;

    cstring = "null devices";
    command = getadbOutput(cstring);

    mstringlist = command.split(QRegExp("[\t\n\r]"), QString::SkipEmptyParts);

    if (command.contains("List of devices attached"))
    {
        mstringlist.removeFirst();
        for (int a = 0; a < mstringlist.size(); a = a + 2)
        {
            QStringList pieces = mstringlist.at(a).split(":", QString::SkipEmptyParts);
            if (!mstringlist.at(a).contains("daemon"))
                dstringlist << pieces.at(0);
        }
    }

}
