#include "adbprefdialog.h"
#include "ui_adbprefdialog.h"
#include <QFileDialog>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtNetwork/QNetworkInterface>
#include <QMessageBox>
#include <QElapsedTimer>
#include <QDesktopServices>
#include <QDir>


#ifdef Q_OS_LINUX
 int osp=0;
#elif defined(Q_OS_WIN)
  int osp=1;
#elif defined(Q_OS_MAC)
int osp=2;
#endif


adbprefDialog::adbprefDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::adbprefDialog)
{
    ui->setupUi(this);

    this->setFixedHeight(360);
    this->setFixedWidth(550);


}
adbprefDialog::~adbprefDialog()
{
    delete ui;
}

void adbprefDialog::on_checkButton_clicked()
{
    QNetworkRequest request;
    request.setUrl(QUrl("http://www.jocala.com/version.txt"));

    // Either:
    // 1) Create once in constructor and reuse (recommended)
    // or
    // 2) Assign parent so Qt cleans it up
    QNetworkAccessManager *m_networkManager = new QNetworkAccessManager(this);

    QNetworkReply *reply = m_networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, &adbprefDialog::onRequestCompleted);
}

void adbprefDialog::onRequestCompleted()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply)
        return;

    if (reply->error() != QNetworkReply::NoError)
    {
        int err = reply->error();
        QString s2 = QString::number(err);
        QMessageBox::critical(this, "", "Network error: " + s2, QMessageBox::Cancel);
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QString s1(data);
    s1 = strip2(s1);  // Assuming your custom trimming function

    if (version2 != s1)
    {
        QDialog dialog(this);  // Pass parent for modal dialog ownership
        QVBoxLayout layout(&dialog);
        QLabel messageLabel("adbLink version " + s1 + " is ready. Download?");
        layout.addWidget(&messageLabel);

        QHBoxLayout buttonLayout;
        QPushButton yesButton("Yes");
        QPushButton noButton("No");
        QPushButton changelogButton("Changelog");

        buttonLayout.addWidget(&yesButton);
        buttonLayout.addWidget(&noButton);
        buttonLayout.addWidget(&changelogButton);

        layout.addLayout(&buttonLayout);

        QObject::connect(&yesButton, &QPushButton::clicked, [&]() {
            QDesktopServices::openUrl(QUrl("http://www.jocala.com"));
            dialog.close();
        });

        QObject::connect(&noButton, &QPushButton::clicked, [&]() {
            dialog.close();
        });

        QObject::connect(&changelogButton, &QPushButton::clicked, [&]() {
            changelog();
        });

        dialog.exec();
    }
    else
    {
        QMessageBox::information(this, "", "No adbLink update available", QMessageBox::Cancel);
    }

    reply->deleteLater();
}

void adbprefDialog::setversionLabel(const QString &versiontext)
{
    ui->versionLabel->setText("adbLink version: "+ versiontext);
    version2 = versiontext;
}



void adbprefDialog::setmacterm(int macterm)
{
    if (osp == 2)
     ui->macTerm->setCurrentIndex(macterm);
    else
    ui->macTerm->setDisabled(true);
}


void adbprefDialog::setlinterm(int linterm)
{

    if (osp == 0)
     ui->linTerm->setCurrentIndex(linterm);
    else
    ui->linTerm->setDisabled(true);


}


void adbprefDialog::setwinterm(int winterm)
{

    if (osp == 1)
     ui->winTerm->setCurrentIndex(winterm);
    else
    ui->winTerm->setDisabled(true);


}


void adbprefDialog::setdownloaddir(const QString &ddir)
{
    ui->dfilepath->setText(ddir);
}





void adbprefDialog::setinstalldir(const QString &idir)
{
    ui->ifilepath->setText(idir);
}


void adbprefDialog::setbackupdir(const QString &bdir)
{
    ui->bfilepath->setText(bdir);
}






void adbprefDialog::setscrcpyargs(const bool &scrcpyargs)
{
    ui->scrcpyargs->setChecked(scrcpyargs);
}




void adbprefDialog::setstartview(const bool &startview)
{
    ui->startview->setChecked(startview);
}



void adbprefDialog::setscopecheck(const bool &scopecheck)
{
    ui->scopecheck->setChecked(scopecheck);
}



void adbprefDialog::setversioncheck(const bool &versioncheck)
{
    ui->versioncheck->setChecked(versioncheck);
}



bool adbprefDialog::scopecheck() {
   return ui->scopecheck->isChecked();
}





bool adbprefDialog::versioncheck() {
   return ui->versioncheck->isChecked();
}




bool adbprefDialog::scrcpyargs() {
   return ui->scrcpyargs->isChecked();
}




bool adbprefDialog::startview() {
   return ui->startview->isChecked();
}


QString adbprefDialog::downloaddir() {
   return ui->dfilepath->text();
}



QString adbprefDialog::installdir() {
   return ui->ifilepath->text();
}


QString adbprefDialog::backupdir() {
   return ui->bfilepath->text();
}




QString adbprefDialog::linterm() {
   return QString::number(ui->linTerm->currentIndex());
}

QString adbprefDialog::macterm() {
   return QString::number(ui->macTerm->currentIndex());
}

QString adbprefDialog::winterm() {
   return QString::number(ui->winTerm->currentIndex());
}



void adbprefDialog::on_downloadButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Default pull path"),
                                                 ui->dfilepath->text(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty() )
    {
        ui->dfilepath->setText(dir);
    }
}


QString adbprefDialog::strip2 (QString str)
{
   str = str.simplified();
   str.replace( " ", "" );
   return str;
}


void adbprefDialog::on_installButton_clicked()
{

    QString dir = QFileDialog::getExistingDirectory(this, tr("Default APK folder"),
                                                 ui->ifilepath->text(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty() )
    {
        ui->ifilepath->setText(dir);
    }

}


void adbprefDialog::on_backupButton_clicked()
{

    QString dir = QFileDialog::getExistingDirectory(this, tr("Default backup folder"),
                                                 ui->bfilepath->text(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty() )
    {
        ui->bfilepath->setText(dir);
    }

}



void adbprefDialog::changelog()
{
    QString link = "http://jocala.com/changelog.txt";
    QDesktopServices::openUrl(QUrl(link));

}

