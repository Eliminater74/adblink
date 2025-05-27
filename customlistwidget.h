#ifndef CUSTOMLISTWIDGET_H
#define CUSTOMLISTWIDGET_H

#include <QListWidget>
#include <QMenu>
#include <QAction>

class CustomListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit CustomListWidget(QWidget *parent = nullptr);

signals:
    void filesDropped(const QStringList &fileNames, const QString &targetDir);
    void externalFilesDropped(const QStringList &filePaths, const QString &targetDir); 
    void focusRequested(); // For setting hasfocus

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void createContextMenu();
    QMenu *contextMenu;
    QAction *copyAction;
    QAction *moveAction;
    QAction *renameAction;
    QAction *deleteAction;
    QAction *editAction;
    QAction *newAction;
    QAction *mkdirAction;
    QAction *pullAction;
    QAction *homeAction;
    QAction *exitAction;
};

#endif // CUSTOMLISTWIDGET_H
