#include "mainwindow.h"
#include <QApplication>
#include <QStyle>
#include <QDesktopWidget>
#include <QStyleFactory>
#include <QFile>
#include <QFontDatabase>

int main(int argc, char *argv[])
{


    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);

    // Load bundled DejaVuSans font
    int fontId = QFontDatabase::addApplicationFont(":/assets/DejaVuSans.ttf");
    if (fontId != -1) {
        QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont font(family);
        font.setPixelSize(12); // Set global default size
        a.setFont(font);
    }

    // Set Fusion style
    a.setStyle(QStyleFactory::create("Fusion"));

    // Load optional stylesheet
    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        a.setStyleSheet(styleSheet);
        styleFile.close();
    }

    MainWindow w;

    // Center the window
    w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w.size(), a.desktop()->availableGeometry()));
    w.show();

    return a.exec();
}
