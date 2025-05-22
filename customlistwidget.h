#ifndef CUSTOMLISTWIDGET_H
#define CUSTOMLISTWIDGET_H

#include <QListWidget>
#include <QMenu> // Added for context menu
#include <QAction> // Added for menu actions

class CustomListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit CustomListWidget(QWidget *parent = nullptr);

signals:
    void filesDropped(const QStringList &fileNames, const QString &targetDir);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override; // Added for context menu

private:
    void createContextMenu(); // Added to initialize context menu
    QMenu *contextMenu; // Added to store context menu
    QAction *copyAction; // Added for copy action
    QAction *moveAction; // Added for move action
    QAction *renameAction; // Added for rename action
    QAction *deleteAction; // Added for delete action
    QAction *editAction; // Added for edit action
};

#endif // CUSTOMLISTWIDGET_H

