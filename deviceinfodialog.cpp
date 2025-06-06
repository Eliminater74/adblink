#include "deviceinfodialog.h"
#include "ui_deviceinfodialog.h"
#include <QString>
#include <QStringList>
#include <QGridLayout>
#include <QDebug>

QString deviceinfoDialog::pad_qstring(const QString& qstring, int length) {
    return qstring.leftJustified(length, ' ', Qt::AlignLeft);
}

deviceinfoDialog::deviceinfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::deviceinfoDialog)
{
    ui->setupUi(this);
}

deviceinfoDialog::~deviceinfoDialog()
{
    delete ui;
}

void deviceinfoDialog::devinfo(const QStringList &list)
{


    QString archi = list.at(0);
    QString droid = list.at(1);
    QString device = list.at(2);
    QString manufact = list.at(3);
    QString batt = list.at(4);

    bool scoped = (list.at(5).toLower() == "true");
    int av = droid.toInt();


    ui->archilabel->setText("System architecture:   " + archi);

    if (manufact.contains(QLatin1String("amazon"), Qt::CaseInsensitive))
        ui->droidlabel->setText(QLatin1String("Fire OS version:   ") + droid);
    else
        ui->droidlabel->setText(QLatin1String("Android version:   ") + droid);

    ui->devicelabel->setText("Device:   " + device);

    ui->manufactlabel->setText("Manufacturer:   " + manufact);



    ui->batterylabel->setText("Battery level: " + batt);

    if (batt.contains("Unknown"))
     ui->batterylabel->setVisible(false);



    if (av >= 11) {
        if (!scoped)
            ui->scoped->setText("/sdcard/Android/data/ is not restricted by scoped storage.");
        else
            ui->scoped->setText("Scoped storage is in effect. /sdcard/Android/data is sandboxed and can't be accessed by adblink.");
    } else {
        ui->scoped->setText("Scoped storage is not implemented on this device.");
    }


}
