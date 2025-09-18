#include "about.h"
#include <QDesktopServices>
#include <QUrl>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QIcon>

Dialog2::Dialog2(QWidget *parent, const QString &donation) :
    QDialog(parent)
{
    // Remove the help button from the window
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);


    setWindowTitle("About");
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(300, 250);


    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 20, 10, 20);
    layout->setSpacing(10);

   \
    alabel = new QLabel("adblinkX 9999", this);
    alabel->setFont(QFont("Arial", 16));
    alabel->setAlignment(Qt::AlignCenter);
    alabel->setFixedSize(280, 50);
    layout->addWidget(alabel);


    linkLabel = new QLabel(this);
    linkLabel->setText("<a href=\"http://www.jocala.com\">http://www.jocala.com</a>");
    linkLabel->setOpenExternalLinks(true);
    linkLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);
    linkLabel->setAlignment(Qt::AlignCenter);
    linkLabel->setFixedSize(280, 50);
    layout->addWidget(linkLabel);


    donate = new QPushButton(this);
    QPixmap pix(":/assets/donate.png");
    QIcon icon(pix);
    donate->setIcon(icon);
    QPixmap scaledPix = pix.scaled(90, 36, Qt::KeepAspectRatio);
    QSize scaledSize = scaledPix.size();
    donate->setIconSize(scaledSize);
    donate->setFixedSize(100, 40);
    donate->setStyleSheet("QPushButton { margin: 2px; padding: 2px; }");
    donate->setFlat(true);


    thankLabel = new QLabel("Thanks for your donation!", this);
    thankLabel->setAlignment(Qt::AlignCenter);
    thankLabel->setFixedSize(280, 50);
    thankLabel->setWordWrap(true);


    thankLabel2 = new QLabel("Donations fund adblink's development!", this);
    thankLabel2->setAlignment(Qt::AlignCenter);
    thankLabel2->setFixedSize(280, 50);
    thankLabel2->setWordWrap(true);


    if (donation == "jocala.com") {
        donate->setVisible(false);
        thankLabel->setVisible(true);
        thankLabel2->setVisible(false);
    } else {
        donate->setVisible(true);
        thankLabel->setVisible(false);
        thankLabel2->setVisible(true);
    }


    layout->addWidget(donate, 0, Qt::AlignHCenter);
    layout->addWidget(thankLabel);
    layout->addWidget(thankLabel2);


    pushButton = new QPushButton("Close", this);
    pushButton->setFixedSize(80, 30);
    layout->addWidget(pushButton, 0, Qt::AlignHCenter);


    layout->addStretch();


    connect(donate, &QPushButton::clicked, this, &Dialog2::on_donate_clicked);
    connect(pushButton, &QPushButton::clicked, this, &Dialog2::close);
}

Dialog2::~Dialog2()
{

}

void Dialog2::on_donate_clicked()
{
    QString link = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=GKZMW456H6E5W";
    QDesktopServices::openUrl(QUrl(link));
}

void Dialog2::setaLabel(const QString &atext)
{
    alabel->setText(atext);
}
