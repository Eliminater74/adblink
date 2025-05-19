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
    setFixedSize(300, 250);

    // Disable the layout to allow manual positioning
    setLayout

        (nullptr);

    // Center-align labels
    ui->alabel->setAlignment(Qt::AlignCenter);
    ui->thankLabel->setAlignment(Qt::AlignCenter);
    ui->linkLabel->setAlignment(Qt::AlignCenter);

    // Set fixed sizes for labels to fit within dialog
    ui->alabel->setFixedSize(280, 50);
    ui->thankLabel->setFixedSize(280, 50);
    ui->linkLabel->setFixedSize(280, 50);

    // Set up the donate button icon and size
    QPixmap pix(":/assets/donate.png"); // 147x47
    QIcon icon(pix);
    ui->donate->setIcon(icon);

    // Scale the icon to fit within the button, preserving aspect ratio
    QSize scaledSize = pix.scaled(90, 36, Qt::KeepAspectRatio).size(); // Approx. 90x29
    ui->donate->setIconSize(scaledSize);
    ui->donate->setFixedSize(QSize(100, 40)); // Enough room for icon + margins

    // Reduce button margins to maximize icon space
    ui->donate->setStyleSheet("QPushButton { margin: 2px; padding: 2px; }");

    // Set fixed size for close button
    ui->pushButton->setFixedSize(80, 30);

    // Manually position all widgets
    ui->alabel->move(10, 20);
    ui->linkLabel->move(10, 50);
    ui->thankLabel->move(10, 100);
    ui->donate->move((300 - 100) / 2, 100);
    ui->thankLabel2->move(40, 150);
    ui->pushButton->move(110, 200);


    if (donation == "jocala.com") {
        ui->donate->setVisible(false);
        ui->thankLabel->setText("Thanks for your donation!");
        ui->thankLabel->setVisible(true);
        ui->thankLabel2->setVisible(false);
    } else {
        ui->donate->setVisible(true);
        ui->thankLabel->setVisible(false);
        ui->thankLabel2->setText("Donations fund adblink's development!");
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
