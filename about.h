#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>

class Dialog2 : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog2(QWidget *parent = nullptr, const QString &donation = QString());
    ~Dialog2();
    void setaLabel(const QString &atext);

private slots:
    void on_donate_clicked();

private:
    QLabel *alabel;
    QLabel *linkLabel;
    QLabel *thankLabel;
    QLabel *thankLabel2;
    QPushButton *donate;
    QPushButton *pushButton;
};

#endif // ABOUT_H
