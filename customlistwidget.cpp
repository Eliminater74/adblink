#include "customlistwidget.h"
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QDataStream>
#include "usbfiledialog.h"

CustomListWidget::CustomListWidget(QWidget *parent) : QListWidget(parent)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setSelectionMode(QAbstractItemView::MultiSelection);
    qDebug() << "CustomListWidget instantiated for" << this << "dragEnabled:" << dragEnabled()
             << "acceptDrops:" << acceptDrops() << "dragDropMode:" << dragDropMode()
             << "selectionMode:" << selectionMode();

    createContextMenu();
}

void CustomListWidget::createContextMenu()
{
    contextMenu = new QMenu(this);
    copyAction = new QAction("Copy", this);
    moveAction = new QAction("Move", this);
    renameAction = new QAction("Rename", this);
    deleteAction = new QAction("Delete", this);
    editAction = new QAction("Edit", this);
    mkdirAction = new QAction("New Directory", this);
    pullAction = new QAction("Pull", this);
    homeAction = new QAction("Home", this); // Added Home action

    contextMenu->addAction(copyAction);
    contextMenu->addAction(moveAction);
    contextMenu->addAction(renameAction);
    contextMenu->addAction(deleteAction);
    contextMenu->addAction(editAction);
    contextMenu->addSeparator();
    contextMenu->addAction(mkdirAction);
    contextMenu->addAction(pullAction);
    contextMenu->addAction(homeAction);

    // Connect actions to usbfileDialog slots
    usbfileDialog *parentDialog = qobject_cast<usbfileDialog*>(parent());
    if (parentDialog) {
        connect(copyAction, &QAction::triggered, parentDialog, &usbfileDialog::on_copyButton_clicked);
        connect(moveAction, &QAction::triggered, parentDialog, &usbfileDialog::on_moveButton_clicked);
        connect(renameAction, &QAction::triggered, parentDialog, &usbfileDialog::on_renameButton_clicked);
        connect(deleteAction, &QAction::triggered, parentDialog, &usbfileDialog::on_delButton_clicked);
        connect(editAction, &QAction::triggered, parentDialog, &usbfileDialog::on_editButton_clicked);
        connect(mkdirAction, &QAction::triggered, parentDialog, &usbfileDialog::on_mkdirButton_clicked);
        connect(pullAction, &QAction::triggered, parentDialog, &usbfileDialog::on_pullButton_clicked);
        connect(homeAction, &QAction::triggered, parentDialog, &usbfileDialog::on_resetButton_clicked);
        qDebug() << "Context menu actions connected to usbfileDialog slots, including mkdir, pull, and home";
    } else {
        qDebug() << "Warning: Parent is not a usbfileDialog, context menu actions not connected";
    }
}

void CustomListWidget::contextMenuEvent(QContextMenuEvent *event)
{
    emit focusRequested(); // Emit signal to set hasfocus

    bool singleSelection = selectedItems().count() == 1;
    renameAction->setEnabled(singleSelection);
    editAction->setEnabled(singleSelection);
    copyAction->setEnabled(!selectedItems().isEmpty());
    moveAction->setEnabled(!selectedItems().isEmpty());
    deleteAction->setEnabled(!selectedItems().isEmpty());
    mkdirAction->setEnabled(true); // Always enabled
    pullAction->setEnabled(!selectedItems().isEmpty()); // Enabled only with selection
    homeAction->setEnabled(true); // Always enabled

    qDebug() << "Showing context menu at position:" << event->globalPos();
    contextMenu->exec(event->globalPos());
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
        QSet<int> processedRows;
        while (!stream.atEnd()) {
            int row, col;
            QMap<int, QVariant> roleDataMap;
            stream >> row >> col >> roleDataMap;
            if (roleDataMap.contains(Qt::DisplayRole) && !processedRows.contains(row)) {
                QString fileName = roleDataMap[Qt::DisplayRole].toString();
                if (fileName != "..") {
                    fileNames << fileName;
                    processedRows.insert(row);
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
