#ifndef CUSTOMLISTWIDGET_H
#define CUSTOMLISTWIDGET_H

#include <QListWidget>

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
};

#endif // CUSTOMLISTWIDGET_H
