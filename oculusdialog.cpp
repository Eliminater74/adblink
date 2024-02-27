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


//qDebug() << "textureBox type:" << typeid(ui->textureBox).name();
//qDebug() << "lineEdit type:" << typeid(ui->textureBox->lineEdit()).name();



}

oculusDialog::~oculusDialog()
{
    delete ui;
}



void oculusDialog::cwidthSet(QString cwidth) {
   ui->customWidth->setText(cwidth);
}


void oculusDialog::cheightSet(QString cheight) {
   ui->customHeight->setText(cheight);
}



void oculusDialog::chromaticSet(int index) {
    ui->chromaticBox->setCurrentIndex(index);
}



void oculusDialog::ratecapSet(int index) {
    ui->ratecaptureBox->setCurrentIndex(index);
}


void oculusDialog::dynamicfovSet(int index) {
    ui->foveation2Box->setCurrentIndex(index);
}

void oculusDialog::fovlevelSet(int index) {
    ui->foveation1Box->setCurrentIndex(index);
}


void oculusDialog::powerSet(int index) {
    ui->powerBox->setCurrentIndex(index);
}



void oculusDialog::proximitySet(int index) {
    ui->proximityBox->setCurrentIndex(index);
}



void oculusDialog::guardianSet(int index) {
    ui->guardianBox->setCurrentIndex(index);
}



void oculusDialog::cpuSet(int index) {
    ui->cpulevelBox->setCurrentIndex(index);
}



void oculusDialog::gpuSet(int index) {
    ui->gpulevelBox->setCurrentIndex(index);
}



void oculusDialog::refreshSet(int index) {
    ui->refreshBox->setCurrentIndex(index);
}



void oculusDialog::textureSet(int index) {
    ui->textureBox->setCurrentIndex(index);
}




void oculusDialog::recordingSet(int index) {
    ui->resolutionBox->setCurrentIndex(index);
}






int  oculusDialog::textureSelected() const {
    return ui->textureBox->currentIndex();
}

int  oculusDialog::recordingSelected()  const {
   return ui->resolutionBox->currentIndex();
}

int  oculusDialog::fovlevelSelected()  const {
    return ui->foveation1Box->currentIndex();
}

int  oculusDialog::dynamicfovSelected() const {
    return ui->foveation2Box->currentIndex();
}

int  oculusDialog::ratecapSelected() const {
    return ui->ratecaptureBox->currentIndex();

}

int  oculusDialog::chromaticSelected() const {
    return ui->chromaticBox->currentIndex();
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


QString oculusDialog::cheightSelected() const {
    return ui->customHeight->text();
}



QString oculusDialog::cwidthSelected() const {
    return ui->customWidth->text();
}



void oculusDialog::setbattery1label(const QString &b1)
{
    ui->battery1Label->setText(b1);
}

/*
}

void YourDialogClass::onSaveButtonClicked()
{
    clickedButton = QDialog::CustomButton1;  // Set to a unique value for Save
    accept();
}
 */



void oculusDialog::on_cancelButton_clicked()
{
    clickedButton=2;
    reject();
}


void oculusDialog::on_saveButton_clicked()
{

    //qDebug() << ui->textureBox->lineEdit()->text();
    clickedButton=1;
    accept();
}


void oculusDialog::on_okButton_clicked()
{

    //qDebug() << ui->textureBox->lineEdit()->text();

     clickedButton=0;
    accept();
}

