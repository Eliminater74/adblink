#include "mainwindow.h"
#include <QApplication>
#include <QStyle>
#include <QDesktopWidget>
#include <QStyleFactory>
#include <QFile>

int main(int argc, char *argv[])
{
    // Enable high-DPI scaling (already present)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);

    // Set Fusion style for a cross-platform, modern look
    a.setStyle(QStyleFactory::create("Fusion"));

    // Load custom QSS for macOS-like appearance
    QFile styleFile(":/style.qss"); // Assuming style.qss is in Qt resources
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        a.setStyleSheet(styleSheet);
        styleFile.close();
    }

    // Set global font (already present, kept as is)
    QFont font = a.font();
    font.setPixelSize(12);
    a.setFont(font);

    MainWindow w;

    // Center the window (already present, kept as is)
    w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w.size(), a.desktop()->availableGeometry()));

    w.show();

    return a.exec();
}
