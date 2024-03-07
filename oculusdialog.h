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


     int  textureSelected() const;
     int  recordingSelected() const;
     int  fovlevelSelected() const;
     int  dynamicfovSelected() const;
     int  ratecapSelected() const;
     int  chromaticSelected() const;

     QString  cheightSelected() const;
     QString  cwidthSelected() const;
     QString  bitrateSelected() const;

     QString fovdownSelected() const;
     QString fovupSelected() const;
     QString fovoutSelected() const;
     QString fovinSelected() const;

     void fovdownSet(QString fovdown);
     void fovupSet(QString fovup);
     void fovoutSet(QString fovout);
     void fovinSet(QString fovin);

     void  powerSet(int index);
     void  proximitySet(int index);
     void  guardianSet(int index);
     void  cpuSet(int index);
     void  gpuSet(int index);
     void  refreshSet(int index);


     void  textureSet(int index);
     void  recordingSet(int index);
     void  fovlevelSet(int index);
     void  dynamicfovSet(int index);
     void  ratecapSet(int index);
     void  chromaticSet(int index);

     void  cwidthSet(QString cwidth);
     void  cheightSet(QString cheight);
     void  bitrateSet(QString cheight);

     void  titleSet(QString title);





     int clickedButton;

 private slots:
     void on_cancelButton_clicked();

     void on_saveButton_clicked();

     void on_okButton_clicked();

 private:
    Ui::oculusDialog *ui;


};

#endif // OCULUSDIALOG_H
