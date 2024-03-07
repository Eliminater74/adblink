#include "oculusdialog.h"
#include "ui_oculusdialog.h"
#include <QDebug>
#include <QLineEdit>

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

connect(ui->okButton, &QPushButton::clicked, this, &oculusDialog::on_okButton_clicked);
connect(ui->saveButton, &QPushButton::clicked, this, &oculusDialog::on_saveButton_clicked);
connect(ui->cancelButton, &QPushButton::clicked, this, &oculusDialog::on_cancelButton_clicked);


}

oculusDialog::~oculusDialog()
{
    delete ui;
}



void oculusDialog::titleSet(QString title) {
    ui->title->setText(title);
}



void oculusDialog::setbattery1label(const QString &b1)
{
    ui->battery1Label->setText(b1);
}


// setprop debug.oculus.capture.textureWidth

int  oculusDialog::textureSelected() const {
    return ui->textureBox->currentIndex();
}



void oculusDialog::textureSet(int index) {
    ui->textureBox->setCurrentIndex(index);
}


// setprop debug.oculus.eyeFovDown

QString oculusDialog::fovdownSelected() const {
    return ui->fovdown->text();
}



void oculusDialog::fovdownSet(QString fovdown) {
    ui->fovdown->setText(fovdown);
}




// setprop debug.oculus.eyeFovUp

void oculusDialog::fovupSet(QString fovup) {
    ui->fovup->setText(fovup);
}


QString oculusDialog::fovupSelected() const {
    return ui->fovup->text();
}




// setprop debug.oculus.eyeFovOutward

void oculusDialog::fovoutSet(QString fovout) {
    ui->fovout->setText(fovout);
}


QString oculusDialog::fovoutSelected() const {
    return ui->fovout->text();
}

// setprop debug.oculus.eyeFovInward

QString oculusDialog::fovinSelected() const {
    return ui->fovin->text();
}


void oculusDialog::fovinSet(QString fovin) {
    ui->fovin->setText(fovin);
}



// setprop debug.oculus.capture.width

QString oculusDialog::cwidthSelected() const {
    return ui->customWidth->text();
}


void oculusDialog::cwidthSet(QString cwidth) {
   ui->customWidth->setText(cwidth);
}




// setprop debug.oculus.capture.bitrate

void oculusDialog::bitrateSet(QString bitrate) {
   ui->customBitrate->setText(bitrate);
}




QString oculusDialog::bitrateSelected() const {
   return ui->customBitrate->text();
}




// setprop debug.oculus.capture.textureHeight

QString oculusDialog::cheightSelected() const {
   return ui->customHeight->text();
}



void oculusDialog::cheightSet(QString cheight) {
   ui->customHeight->setText(cheight);
}



// setprop debug.oculus.forceChroma

void oculusDialog::chromaticSet(int index) {
    ui->chromaticBox->setCurrentIndex(index);
}



int  oculusDialog::chromaticSelected() const {
    return ui->chromaticBox->currentIndex();
}


// setprop debug.oculus.fullRateCapture

void oculusDialog::ratecapSet(int index) {
    ui->ratecaptureBox->setCurrentIndex(index);
}



int  oculusDialog::ratecapSelected() const {
    return ui->ratecaptureBox->currentIndex();

}


//  setprop debug.oculus.foveation.dynamic

int  oculusDialog::dynamicfovSelected() const {
    return ui->foveation2Box->currentIndex();
}


void oculusDialog::dynamicfovSet(int index) {
    ui->foveation2Box->setCurrentIndex(index);
}


// setprop debug.oculus.foveation.level


void oculusDialog::fovlevelSet(int index) {
    ui->foveation1Box->setCurrentIndex(index);
}


int  oculusDialog::fovlevelSelected()  const {
    return ui->foveation1Box->currentIndex();
}

// svc power stayon

void oculusDialog::powerSet(int index) {
    ui->powerBox->setCurrentIndex(index);
}



int oculusDialog::powerSelected() const {
    return ui->powerBox->currentIndex();
}

//  am broadcast -a com.oculus.vrpowermanager.prox_close
//  am broadcast -a com.oculus.vrpowermanager.automation_disable

void oculusDialog::proximitySet(int index) {
    ui->proximityBox->setCurrentIndex(index);
}



int oculusDialog::proximitySelected() const {
    return ui->proximityBox->currentIndex();
}


//  setprop debug.oculus.guardian_pause


void oculusDialog::guardianSet(int index) {
    ui->guardianBox->setCurrentIndex(index);
}



int oculusDialog::guardianSelected() const {
    return ui->guardianBox->currentIndex();
}


//  setprop debug.oculus.cpuLevel


void oculusDialog::cpuSet(int index) {
    ui->cpulevelBox->setCurrentIndex(index);
}




int oculusDialog::cpuSelected() const {
    return ui->cpulevelBox->currentIndex();
}


//  setprop debug.oculus.gpuLevel

int oculusDialog::gpuSelected() const {
    return ui->gpulevelBox->currentIndex();
}

void oculusDialog::gpuSet(int index) {
    ui->gpulevelBox->setCurrentIndex(index);
}


// setprop debug.oculus.refreshrate

void oculusDialog::refreshSet(int index) {
    ui->refreshBox->setCurrentIndex(index);
}




int oculusDialog::refreshSelected() const {
    return ui->refreshBox->currentIndex();
}



// setprop debug.oculus.capture.width
// setprop debug.oculus.capture.height

void oculusDialog::recordingSet(int index) {
    ui->resolutionBox->setCurrentIndex(index);
}





int  oculusDialog::recordingSelected()  const {
   return ui->resolutionBox->currentIndex();
}





void oculusDialog::on_cancelButton_clicked()
{
    clickedButton=2;
    reject();
}


void oculusDialog::on_saveButton_clicked()
{

    clickedButton=1;
    accept();
}


void oculusDialog::on_okButton_clicked()
{

     clickedButton=0;
    accept();
}

