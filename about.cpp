#include "about.h"
#include "ui_about.h"
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QLayout>

Dialog2::Dialog2(QWidget *parent, const QString &donation) :
    QDialog(parent),
    ui(new Ui::Dialog2)
{
    ui->setupUi(this);

    // Remove the help button from the window
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Set size policy to fixed to prevent resizing
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Set fixed size for the dialog (client area)
    setFixedSize(300, 300);

    // Disable the layout to allow manual positioning
    setLayout(nullptr);

    // Center-align labels
    ui->alabel->setAlignment(Qt::AlignCenter);
    ui->thankLabel->setAlignment(Qt::AlignCenter);
    ui->linkLabel->setAlignment(Qt::AlignCenter);

    // Set fixed sizes for labels to fit within dialog
    ui->alabel->setFixedSize(280, 50);
    ui->thankLabel->setFixedSize(280, 50);
    ui->linkLabel->setFixedSize(280, 50);

    // Set up the donate button icon and size
    QPixmap pix(":/assets/donate.png");
    QIcon icon(pix);
    ui->donate->setIcon(icon);
    ui->donate->setIconSize(pix.size());
    ui->donate->setFixedSize(pix.size().boundedTo(QSize(100, 40)));

    // Set fixed size for close button
    ui->pushButton->setFixedSize(80, 30);

    // Manually position all widgets
    ui->alabel->move(10, 20);
    ui->linkLabel->move(10, 50);

     ui->thankLabel->move(10, 100);
     ui->donate->move(100, 100);
     ui->thankLabel2->move(25, 150);


     ui->pushButton->move(110, 240); // Below donate

    // Configure UI based on donation value
    if (donation == "jocala.com") {
        ui->donate->setVisible(false);
        ui->thankLabel->setText("Thanks for your donation!");
        ui->thankLabel->setVisible(true);
        ui->thankLabel2->setVisible(false);
    } else {
        ui->donate->setVisible(true);
        ui->thankLabel->setVisible(false);
        ui->thankLabel2->setVisible(true);
    }


}

Dialog2::~Dialog2()
{
    delete ui;
}

void Dialog2::on_donate_clicked()
{
    QString link = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=GKZMW456H6E5W";
    QDesktopServices::openUrl(QUrl(link));
}

void Dialog2::on_pushButton_clicked()
{
    // Placeholder for other button functionality
}

void Dialog2::setaLabel(const QString &atext)
{
    ui->alabel->setText(atext);
}
