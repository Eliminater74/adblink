#ifndef OCULUSDIALOG_H
#define OCULUSDIALOG_H

#include <QDialog>

namespace Ui {
class oculusDialog;
}

class oculusDialog : public QDialog
{
    Q_OBJECT

public:
    explicit oculusDialog(QWidget *parent = nullptr);
    ~oculusDialog();


    void setbattery1label(const QString &b1);

    int  powerSelected() const;


    int  proximitySelected() const;
    int  guardianSelected() const;

     int  cpuSelected() const;
     int  gpuSelected() const;
     int  refreshSelected() const;


private slots:

private:
    Ui::oculusDialog *ui;
};

#endif // OCULUSDIALOG_H
