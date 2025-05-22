#include "customlistwidget.h"
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QDataStream>

CustomListWidget::CustomListWidget(QWidget *parent) : QListWidget(parent)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    qDebug() << "CustomListWidget instantiated for" << this << "dragEnabled:" << dragEnabled()
             << "acceptDrops:" << acceptDrops() << "dragDropMode:" << dragDropMode();
}

void CustomListWidget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "mousePressEvent called, pos:" << event->pos();
    QListWidgetItem *item = itemAt(event->pos());
    if (item) {
        qDebug() << "Clicked item:" << item->text() << "flags:" << item->flags()
                 << "dragEnabled:" << (item->flags() & Qt::ItemIsDragEnabled);
    } else {
        qDebug() << "No item at position";
    }
    QListWidget::mousePressEvent(event);
}

void CustomListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "dragEnterEvent called, mimeData formats:" << event->mimeData()->formats();
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        event->acceptProposedAction();
    } else {
        qDebug() << "dragEnterEvent ignored: no supported mime data";
        event->ignore();
    }
}

void CustomListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    qDebug() << "dragMoveEvent called";
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void CustomListWidget::dropEvent(QDropEvent *event)
{
    qDebug() << "dropEvent called, mimeData formats:" << event->mimeData()->formats();
    QStringList fileNames;
    QString targetDir = property("currentDirectory").toString();
    qDebug() << "dropEvent: targetDir from currentDirectory:" << targetDir;

    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        QByteArray modelData = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
        QDataStream stream(&modelData, QIODevice::ReadOnly);
        while (!stream.atEnd()) {
            int row, col;
            QMap<int, QVariant> roleDataMap;
            stream >> row >> col >> roleDataMap;
            if (roleDataMap.contains(Qt::DisplayRole)) {
                QString fileName = roleDataMap[Qt::DisplayRole].toString();
                if (fileName != "..") {
                    fileNames << fileName;
                }
            }
        }
        qDebug() << "Model mime data: fileNames:" << fileNames;
    }

    if (!fileNames.isEmpty() && !targetDir.isEmpty()) {
        qDebug() << "Drop event: fileNames:" << fileNames << "targetDir:" << targetDir;
        emit filesDropped(fileNames, targetDir);
        qDebug() << "Emitted filesDropped signal";
        event->acceptProposedAction();
    } else {
        qDebug() << "Drop ignored: fileNames:" << fileNames << "targetDir:" << targetDir;
        event->ignore();
    }
}
