#ifndef CONNECTADB_H
#define CONNECTADB_H

#include <QStringList>
#include <QString>

// QString connectadb(const QString &cstring);

QString connectadb(const QString &adbPath, const QStringList &arguments);



#endif // CONNECTADB_H
