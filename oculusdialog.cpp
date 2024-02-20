#include "oculusdialog.h"
#include "ui_oculusdialog.h"
#include <QDebug>

int xandos;
bool guardian;
bool proximity;
bool screentimer;

oculusDialog::oculusDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::oculusDialog)
{
    ui->setupUi(this);

    this->setWindowTitle("Quest Settings");

   // ui->screenEdit->setInputMask("#9999999999");
   // ui->guardianEdit->setInputMask("#9999999999");


}

oculusDialog::~oculusDialog()
{
    delete ui;
}





int oculusDialog::proximitySelected() const {
    return ui->proximityBox->currentIndex();
}


int oculusDialog::guardianSelected() const {
    return ui->guardianBox->currentIndex();
}





int oculusDialog::powerSelected() const {
    return ui->powerBox->currentIndex();
}


int oculusDialog::cpuSelected() const {
    return ui->cpulevelBox->currentIndex();
}



int oculusDialog::gpuSelected() const {
    return ui->gpulevelBox->currentIndex();
}



int oculusDialog::refreshSelected() const {
    return ui->refreshBox->currentIndex();
}





void oculusDialog::setbattery1label(const QString &b1)
{
    ui->battery1Label->setText(b1);
}


/*

void oculusDialog::on_guardianOff_clicked()
{

 //           ui->guardianOff->setChecked(true);
 //           ui->guardianOn->setChecked(true);
            guardian=false;




}

void oculusDialog::on_guardianOn_clicked()
{

  //          ui->guardianOff->setChecked(false);
//            ui->guardianOn->setChecked(true);
            guardian=true;

}




void oculusDialog::on_screenOff_clicked()
{


 //   ui->screenOff->setChecked(true);
 //   ui->screenOn->setChecked(false);
    screentimer=false;


}

void oculusDialog::on_screenOn_clicked()
{

 //   ui->screenOff->setChecked(false);
 //   ui->screenOn->setChecked(true);
    screentimer=true;

}


void oculusDialog::on_proximityOff_clicked()
{

 //   ui->proximityOff->setChecked(true);
 //   ui->proximityOn->setChecked(false);
    proximity=false;

}


void oculusDialog::on_proximityOn_clicked()
{

  //  ui->proximityOff->setChecked(false);
   // ui->proximityOn->setChecked(true);
    proximity=true;

}


void oculusDialog::on_defaultButton_clicked()
{
    
    ui->proximityBox2->setChecked(false);
    ui->guardianBox2->setChecked(false);

}

*/
