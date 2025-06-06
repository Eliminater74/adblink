    #include "mainwindow.h"
    #include "ui_mainwindow.h"
    #include "about.h"
    #include "helpdialog.h"
    #include "connectadb.h"
    #include "uninstalldialog.h"
    #include "getreturncode.h"
    #include "editordialog.h"
    #include "keyboarddialog.h"
    #include "usbfiledialog.h"
    #include "listfiledialog.h"
    #include "deviceinfodialog.h"
    #include "cachedialog.h"
    #include "datadialog.h"
    #include "backupdialog.h"
    #include "forcequitdialog.h"
    #include "restdialog.h"
    #include "klogdialog.h"
    #include "tcpipdialog.h"
    #include "adbprefdialog.h"
    #include "sleepdialog.h"
    #include "oculusdialog.h"
    #include "scpdialog.h"
    #include "program.h"
    #include "getadbdata.h"
    #include "logfile.h"
    #include "setpdialog.h"
    #include "adbutils.h"
    #include "getlocaladb.h"

    #ifdef __WIN32__
      #include "windows.h"
    #endif


    #include <QtConcurrent/QtConcurrent>
    #include <qtconcurrentrun.h>
    #include <QMessageBox>
    #include <QTableWidget>
    #include <QResource>
    #include <QProcess>
    #include <QDir>
    #include <QFileDialog>
    #include <QThread>
    #include <QFile>
    #include <QProgressBar>
    #include <QTimer>
    #include <QLabel>
    #include <QInputDialog>
    #include <QtSql>
    #include <QtSql/QSqlDatabase>
    #include <QtSql/QSqlError>
    #include <QtSql/QSqlQuery>
    #include <preferencesdialog.h>
    #include <QElapsedTimer>
    #include <QTextStream>
    #include <QDate>
    #include <QRegularExpression>
    #include <QStringList>
    #include <QListWidget>
    #include <QListWidgetItem>
    #include <QDesktopServices>
    #include <QUrl>
    #include <QPixmap>
    #include <QIcon>
    #include <QObject>
    #include <QNetworkAccessManager>
    #include <QNetworkRequest>
    #include <QNetworkReply>
    #include <QtNetwork>
    #include <QtNetwork/QNetworkInterface>
    #include <adblogdialog.h>
    #include <QFileInfo>
    #include <QThread>
    #include <QFileInfo>
    #include<QClipboard>
    #include <QShortcut>
    #include <QDebug>
    #include <QRegularExpression>
    #include <QStackedWidget>
    #include <QGridLayout>
    #include <QVBoxLayout>
    #include <QHBoxLayout>


    #include <QScrollBar>
    #include <QtGlobal>

    #ifdef Q_OS_LINUX
     int os=0;
    #elif defined(Q_OS_WIN)
      int os=1;
    #elif defined(Q_OS_MAC)
    int os=2;
    #endif



        MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
        , m_networkManager(new QNetworkAccessManager(this))
    {



        adbfiles=QCoreApplication::applicationDirPath()+"/adbfiles/";
        aapt=QCoreApplication::applicationDirPath()+"/adbfiles/"+"aapt";
        aapt = '"'+aapt+'"';
        apphome = QCoreApplication::applicationDirPath();
        scrcpydir=QCoreApplication::applicationDirPath()+"/adbfiles/"+"scrcpy/";
        xmldir = adbfiles+"remotes/";
        splashdir = adbfiles+"splash/";


        if (!QFile::exists(adbfiles + "adb") && !QFile::exists(adbfiles + "adb.exe")) {
            QMessageBox::critical(0, "", "adb binary missing!\n", QMessageBox::Cancel);
            return;
        }


        if (os == 1) {
            databasedir = QDir::homePath() + "/AppData/Roaming/.jocala/";
        } else  {
            databasedir = QDir::homePath() + "/.jocala/";
        }

        scriptdir = databasedir + "scripts/";
        dbstring = databasedir + "adblink.db";
        jsonstring = databasedir + "adblink.json";
        logfiledir = databasedir;

        QDir dir(databasedir);
        if (!dir.exists()) {
            dir.mkpath(".");
        }


        dir = QDir(scriptdir);
        if (!dir.exists()) {
            dir.mkpath(".");

        }



        connect(qApp, &QCoreApplication::aboutToQuit, this, &MainWindow::onApplicationQuit);


        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);


         ui->setupUi(this);
         setFixedSize(size());
         setWindowTitle(" ");



    #ifdef Q_OS_MAC
      //https://bugreports.qt.io/browse/QTBUG-51120
      ui->progressBar->setTextVisible(true);
    #endif //Q_OS_MAC



         ui->statusBar->addPermanentWidget(ui->server_running);
         ui->statusBar->addPermanentWidget(ui->progressBar);
         ui->progressBar->setHidden(true);






      rotate_logfile();


      QDateTime dateTime = QDateTime::currentDateTime();
      QString dtstr = dateTime.toString("MM/dd/yy hh:mm:ss");


      logfile(program+" "+version);
      logfile(QCoreApplication::applicationDirPath());
      logfile(dtstr);
      logfile(QDir::home().path());



         QList<QHostAddress> list = QNetworkInterface::allAddresses();
         QHostAddress primaryIP;


         for (const QHostAddress& address : list) {
             if (!address.isLoopback() && address.protocol() == QAbstractSocket::IPv4Protocol) {
                 if (address != QHostAddress::LocalHost) {
                     primaryIP = address;
                     logfile("IP:"+primaryIP.toString());
                    break;
                 }
             }
         }


         if (os == 1) {
             logfile("Windows");
         } else if (os == 2) {
             logfile("macOS");
         } else if (os == 0) {
             logfile("Linux");
         } else {
             logfile("Unknown OS");
         }
         logfile("------------");


         dbstring = databasedir + "adblink.db";
         QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
         db.setDatabaseName(dbstring);


         if (!db.open()) {
             QString errorMsg = db.lastError().text();
             logfile(QString("Error opening database: %1 - %2").arg(dbstring, errorMsg));
             QMessageBox::critical(0, qApp->tr("Cannot open database"),
                                   QString("Failed to open database:\n%1\nError: %2").arg(dbstring, errorMsg),
                                   QMessageBox::Cancel);
             return;
         }



         createTables();


         if (!QFileInfo::exists(databasedir+"/adblink.json"))
             {
                     QJsonObject obj;

                     obj["checkversion"] = true;
                     obj["scrcpy"] = true;
                     obj["startview"] = true;
                     obj["dropdown"] = "0";
                     obj["download"] = QDir::homePath();
                     obj["install"] = QDir::homePath();
                     obj["backup"] = QDir::homePath();
                     obj["donation"] = "";
                     obj["localadb"] = "";
                     obj["stopapp"] = "org.xbmc.kodi";
                     obj["startapp"] = "org.xbmc.kodi/org.xbmc.kodi.Splash";

                     QJsonDocument doc(obj);

                    QFile file(databasedir+"adblink.json");
                    file.open(QIODevice::WriteOnly);
                    file.write(doc.toJson());
                    file.close();
           }





        setFixedSize(575,390);
        buttonsetup();
        gridsetup();
        setupDonateButton(ui->centralWidget, 132, 315);

        QString donation = readDonationValue();
        setDonateButtonActive(donation != "jocala.com");

        ui->server_running->setText("");

        connections();
        loadDeviceTable();
        do_versioncheck();



     }


    MainWindow::~MainWindow()
    {




       delete ui;
    }


    void MainWindow::onApplicationQuit() {


     QString cstring = getadbpath() + " kill-server";
     QString command=getadbOutput(cstring);
     logfile("server stopped");


       QDir dir(scriptdir);
       dir.setNameFilters(QStringList());
       dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot);
       foreach(const QString& dirFile, dir.entryList()) {
                    QString filePath = dir.absoluteFilePath(dirFile);
                    QFile file(filePath);
                    file.setPermissions(QFile::WriteUser | QFile::ReadUser);
                    file.remove();
       }
    }


/////////////////////////////////


    void MainWindow::buttonsetup()

    {
         ui->fmButton->setFixedSize(120,32);
         ui->adbshellButton1->setFixedSize(120,32);
         ui->backupButton->setFixedSize(120,32);
         ui->restoreButton->setFixedSize(120,32);
         ui->sideload_Button->setFixedSize(120,32);
         ui->uninstall_Button->setFixedSize(120,32);
         ui->mvdataButton->setFixedSize(120,32);
         ui->pushTimers->setFixedSize(120,32);
         ui->screencap1->setFixedSize(120,32);
         ui->stopADB->setFixedSize(120,32);
         ui->scpyButton->setFixedSize(120,32);
         ui->cacheButton->setFixedSize(120,32);
         ui->doConsole->setFixedSize(120,32);
         ui->keypadButton->setFixedSize(120,32);
         ui->startapp->setFixedSize(120,32);
         ui->stopapp->setFixedSize(120,32);
         ui->fmButton_2->setFixedSize(120,32);
         ui->sideload_Button_2->setFixedSize(120,32);
         ui->uninstall_Button_2->setFixedSize(120,32);
         ui->infoButton->setFixedSize(120,32);
         ui->screencap2->setFixedSize(120,32);
         ui->stopADB2->setFixedSize(120,32);
         ui->startapp_2->setFixedSize(120,32);
         ui->stopapp_2->setFixedSize(120,32);
         ui->adbshellButton_2->setFixedSize(120,32);
         ui->doConsole_2->setFixedSize(120,32);
         ui->textButton->setFixedSize(120,32);
         ui->scpyButton_2->setFixedSize(120,32);

         ui->connButton->setFixedSize(80,32);
         ui->disButton->setFixedSize(80,32);
         ui->newRecordButton->setFixedSize(80,32);
         ui->editRecordButton->setFixedSize(80,32);
         ui->delRecordButton->setFixedSize(80,32);
         ui->clearAdhocButton->setFixedSize(80,32);

         ui->newRecordButton->setShortcut(QKeySequence("Ctrl+B"));
         ui->editRecordButton->setShortcut(QKeySequence("Ctrl+C"));
         ui->delRecordButton->setShortcut(QKeySequence("Ctrl+D"));
         ui->connButton->setShortcut(QKeySequence("Ctrl+E"));
         ui->disButton->setShortcut(QKeySequence("Ctrl+F"));
         ui->fmButton->setShortcut(QKeySequence("Ctrl+G"));
         ui->adbshellButton1->setShortcut(QKeySequence("Ctrl+I"));
         ui->sideload_Button->setShortcut(QKeySequence("Ctrl+L"));
         ui->uninstall_Button->setShortcut(QKeySequence("Ctrl+M"));
         ui->scpyButton->setShortcut(QKeySequence("Ctrl+R"));
         ui->doConsole->setShortcut(QKeySequence("Ctrl+T"));
         ui->stopapp->setShortcut(QKeySequence("Ctrl+U"));
         ui->startapp->setShortcut(QKeySequence("Ctrl+W"));
         ui->clearAdhocButton->setShortcut(QKeySequence("Ctrl+Y"));
         ui->stopADB->setShortcut(QKeySequence("Ctrl+Z"));
         ui->startapp->setShortcut(QKeySequence("Ctrl+V"));
         ui->mvdataButton->setShortcut(QKeySequence("Ctrl+N"));
         ui->backupButton->setShortcut(QKeySequence("Ctrl+J"));
         ui->restoreButton->setShortcut(QKeySequence("Ctrl+K"));

        new QShortcut (QKeySequence("Ctrl+O"), this, SLOT(on_actionSend_text_triggered()));
        new QShortcut (QKeySequence("Ctrl+X"), this, SLOT(displayOff()));

    }

///////////////////////////////////////////

    void MainWindow::gridsetup()
    {

         QWidget* gridWidget1 = new QWidget();
         QWidget* gridWidget2 = new QWidget();


         QGridLayout* gridLayout1 = new QGridLayout(gridWidget1);
         QGridLayout* gridLayout2 = new QGridLayout(gridWidget2);

         // --------- Fill first grid
         // Row 0
         gridLayout1->addWidget(ui->fmButton,        0, 0);
         gridLayout1->addWidget(ui->adbshellButton1, 0, 1);
         gridLayout1->addWidget(ui->backupButton,    0, 2);
         gridLayout1->addWidget(ui->restoreButton,   0, 3);

         // Row 1
         gridLayout1->addWidget(ui->sideload_Button,  1, 0);
         gridLayout1->addWidget(ui->uninstall_Button, 1, 1);
         gridLayout1->addWidget(ui->mvdataButton,     1, 2);
         gridLayout1->addWidget(ui->pushTimers,       1, 3);

         // Row 2
         gridLayout1->addWidget(ui->screencap1,    2, 0);
         gridLayout1->addWidget(ui->stopADB,       2, 1);
         gridLayout1->addWidget(ui->scpyButton,    2, 2);
         gridLayout1->addWidget(ui->cacheButton,   2, 3);

         // Row 3
         gridLayout1->addWidget(ui->doConsole,     3, 0);
         gridLayout1->addWidget(ui->keypadButton,  3, 1);
         gridLayout1->addWidget(ui->startapp,      3, 2);
         gridLayout1->addWidget(ui->stopapp,       3, 3);

         // --------- Fill second grid
         // Row 0
         gridLayout2->addWidget(ui->fmButton_2,         0, 0);
         gridLayout2->addWidget(ui->sideload_Button_2,  0, 1);
         gridLayout2->addWidget(ui->uninstall_Button_2, 0, 2);
         gridLayout2->addWidget(ui->infoButton,         0, 3);

         // Row 1
         gridLayout2->addWidget(ui->screencap2,   1, 0);
         gridLayout2->addWidget(ui->stopADB2,     1, 1);
         gridLayout2->addWidget(ui->startapp_2,   1, 2);
         gridLayout2->addWidget(ui->stopapp_2,    1, 3);

         // Row 2
         gridLayout2->addWidget(ui->adbshellButton_2, 2, 0);
         gridLayout2->addWidget(ui->doConsole_2,      2, 1);
         gridLayout2->addWidget(ui->textButton,       2, 2);
         gridLayout2->addWidget(ui->scpyButton_2,     2, 3);

         // Row 3 — spacers to match grid1 layout and size
         gridLayout2->addItem(new QSpacerItem(0, 30, QSizePolicy::Minimum, QSizePolicy::Fixed), 3, 0);
         gridLayout2->addItem(new QSpacerItem(0, 30, QSizePolicy::Minimum, QSizePolicy::Fixed), 3, 1);
         gridLayout2->addItem(new QSpacerItem(0, 30, QSizePolicy::Minimum, QSizePolicy::Fixed), 3, 2);
         gridLayout2->addItem(new QSpacerItem(0, 30, QSizePolicy::Minimum, QSizePolicy::Fixed), 3, 3);


         gridWidget1->setContentsMargins(0, 0, 0, 0);
         gridLayout1->setHorizontalSpacing(0);
         gridLayout1->setVerticalSpacing(0);

         gridWidget2->setContentsMargins(0, 0, 0, 0);
         gridLayout2->setHorizontalSpacing(0);
         gridLayout2->setVerticalSpacing(0);


         ui->stackedWidget->layout()->setContentsMargins(0, 0, 0, 0);





         // --------- Clear existing widgets from stackedWidget ---------
         while (ui->stackedWidget->count() > 0) {
      QWidget* w = ui->stackedWidget->widget(0);
      ui->stackedWidget->removeWidget(w);
      delete w;
         }

         // --------- Add the new grid widgets ---------
         ui->stackedWidget->addWidget(gridWidget1);
         ui->stackedWidget->addWidget(gridWidget2);

         // --------- Show the first grid ---------
         ui->stackedWidget->setCurrentIndex(0);


  }

  //////////////////////////////////////////////

  QPushButton* MainWindow::setupDonateButton(QWidget* parent, int x, int y) {
         donateButton = new QPushButton(parent); // Assign to member variable
         QPixmap pix(":/assets/donatel.png");
         if (pix.isNull()) {
      qDebug() << "Error: Failed to load :/assets/donatel.png";
      donateButton->setText("Donate");
         } else {
      QIcon icon(pix);
      donateButton->setIcon(icon);
      donateButton->setText("");
      donateButton->setIconSize(QSize(300, 20));
         }
         donateButton->setGeometry(x, y, 300, 20); // Use x, y parameters
         donateButton->setStyleSheet(
             "QPushButton {"
             "   border: none;"
             "   background: transparent;"
             "   padding: 0;"
             "}"
             "QPushButton:hover {"
             "   background: rgba(200, 200, 200, 50);"
             "}"
             );
         if (QMetaObject::checkConnectArgs(SIGNAL(clicked()), SLOT(on_donate_clicked()))) {
      connect(donateButton, &QPushButton::clicked, this, &MainWindow::on_donate_clicked);
         } else {
      qDebug() << "Warning: on_donate_clicked slot not found";
         }

         return donateButton;
    }

    void MainWindow::setDonateButtonActive(bool active) {
         if (donateButton) {
      donateButton->setVisible(active);
      donateButton->setEnabled(active);
         } else {
      qDebug() << "Error: donateButton is not initialized";
         }
    }



    QString MainWindow::readDonationValue() {
         QJsonObject obj;
         QJsonDocument doc;
         QFile file(databasedir + "adblink.json");

         if (file.open(QIODevice::ReadOnly)) {
      doc = QJsonDocument::fromJson(file.readAll());
      obj = doc.object();
      file.close();
      return obj["donation"].toString();
         } else {
      qDebug() << "Error: Could not open adblink.json for reading";
      return QString(); // Return empty string on error
         }
    }

    /////////////////////////////////////////////////////
    void MainWindow::TimerEvent()
    {
      int value = ui->progressBar->value();

      if (value >= 100)
          {
             value = 0;
             ui->progressBar->reset();
         }

      ui->progressBar->setValue(value+1);



    }


    //////////////////////////////////////////////
    int MainWindow::getperms(QString dir)

    {

        QString cstring;
        QString command;

        cstring = getadb() +   " shell stat -c %a "+dir;

        command=getadbOutput(cstring);

        qDebug() << command;

         int permissions = command.toInt();

         if (command.isEmpty()) {
            permissions = 0;
          }


       return permissions;


    }



///////////////////////////////////////

    bool MainWindow::isScoped()
    {


       // Validate getadb()
       QString adbPath = getadb();
       if (adbPath.isEmpty()) {
            logfile("Issue: getadb() returned empty path");
            return false;
       }

       // Helper to run ADB commands
       auto runAdbCommand = [adbPath](const QString& adbCommand) -> QString {
           QString command = adbPath + " " + adbCommand;
           QProcess process;
           process.start(command);
           if (!process.waitForFinished(5000)) {
               logfile("Issue: ADB command timed out: " + command);
               return QString();
           }
           QString output = process.readAllStandardOutput().trimmed();
           QString error = process.readAllStandardError().trimmed();
           if (process.exitCode() != 0 || !error.isEmpty()) {
               logfile("Issue: ADB command failed: " + command + " Error: " + error);
               return error.isEmpty() ? "Unknown error" : error;
           }
           return output;
       };

       // Get API level
       QString apiOutput = runAdbCommand("shell getprop ro.build.version.sdk");
       bool ok;
       int apiLevel = apiOutput.toInt(&ok);
       if (!ok || apiOutput.isEmpty()) {
            logfile("Issue: Invalid or empty API level output: " + apiOutput);
            return false;
       }
       if (apiLevel < 29) {
            logfile("Issue: API level too low for scoped storage: " + QString::number(apiLevel));
            return false;
       }

       // Test storage access
       bool restrictedAccess = false;
       QString touchOutput = runAdbCommand("shell touch /sdcard/Android/data/org.xbmc.kodi/files/test.txt");
       if (touchOutput.isEmpty() && !restrictedAccess) {
            // Touch succeeded, clean up
            runAdbCommand("shell rm /sdcard/Android/data/org.xbmc.kodi/files/test.txt");
       } else {
            restrictedAccess = touchOutput.contains("Permission denied", Qt::CaseInsensitive);
            if (!restrictedAccess && !touchOutput.isEmpty()) {
                 logfile("Issue: Unexpected touch output for primary path: " + touchOutput);
            }
       }

       // Additional test for another path
       if (!restrictedAccess) {
            touchOutput = runAdbCommand("shell touch /sdcard/DCIM/test.txt");
            if (touchOutput.isEmpty()) {
                 // Touch succeeded, clean up
                 runAdbCommand("shell rm /sdcard/DCIM/test.txt");
            } else {
                 restrictedAccess = touchOutput.contains("Permission denied", Qt::CaseInsensitive);
                 if (!restrictedAccess && !touchOutput.isEmpty()) {
                    logfile("Issue: Unexpected touch output for DCIM path: " + touchOutput);
                 }
            }
       }

       // Check filesystem permissions
       QString lsOutput = runAdbCommand("shell ls -ld /sdcard/");
       if (lsOutput.isEmpty()) {
            logfile("Issue: Failed to get /sdcard/ permissions");
       } else {
            bool permissiveFs = lsOutput.contains("rwxrwxrwx");
            if (permissiveFs) {
                 logfile("Issue: Permissive /sdcard/ permissions, vendor may bypass scoped storage");
                 restrictedAccess = false;
            }
       }

       bool result = (apiLevel >= 30) || (apiLevel == 29 && restrictedAccess);
       logfile(QString("scoped storage is %1").arg(result ? "in effect" : "not in effect"));
       return result;
    }



    //////////////////////////////////////////////
    int MainWindow::getandroid()

    {
        // getprop ro.build.fingerprint

        QString cstring = getadb() +   " shell getprop ro.build.version.release";
        QString command=getadbOutput(cstring);
        return command.toInt();

    }



    //////////////////////////////////////////////
    QString MainWindow::manufacturer()

    {

        QString cstring = getadb() +   " shell getprop ro.product.manufacturer";
        QString manufacturer=getadbOutput(cstring);

        return manufacturer;
    }


    //////////////////////////////////////////////
    QString MainWindow::devicename()

    {

        QString cstring = getadb() +   " shell getprop ro.product.device";
        QString device=getadbOutput(cstring);

        return device;
    }


    /////////////////////////////////////////////

    bool MainWindow::fileExists(QString path) {
        QFileInfo checkFile(path);
        // check if file exists and if yes: Is it really a file and no directory?
        if (checkFile.exists() && checkFile.isFile()) {
            return true;
        } else {
            return false;
        }
    }

    /////////////////////////////////////////
    void MainWindow::delayTimer(int rdelay)
    {

     QElapsedTimer rtimer;

    int nMilliseconds;
     int i = 0;

     rtimer.start();

    while(i == 0)
      {
        qApp->processEvents();
         nMilliseconds = rtimer.elapsed();
       if (nMilliseconds >= rdelay)
           break;
    }


    }


    //////////////////////////////////////////////
    void MainWindow::rotate_logfile()

    {

     QFile file(logfiledir+"adblink.old.log");

     if( file.exists() )
         QFile::remove(logfiledir+"adblink.old.log");


    QFile file2(logfiledir+"adblink.log");

    if( file2.exists() )
        file2.rename(logfiledir+"adblink.old.log");


    }


    //////////////////////////////////////////////
    void MainWindow::logfile2(QString line)

    {


    QFile file(logfiledir+"adblink.log");
       if(!file.open(QFile::WriteOnly | QFile::Text | QFile::Append))
          {
           QMessageBox::critical(0, "","Can't create logfile!\n",QMessageBox::Cancel);
           return;
          }

       QTextStream out(&file);
                out  << line << endl;

     }


    /////////////////////////////////////////
    void MainWindow::rebootDevice(QString reboot)
    {

     QElapsedTimer rtimer;
     int nMilliseconds;

    QProcess reboot_device;
    rtimer.start();
    reboot_device.setProcessChannelMode(QProcess::MergedChannels);

    QString cstring = getadb() + " " + reboot;

    reboot_device.start(cstring);
    reboot_device.waitForStarted();
    while(reboot_device.state() != QProcess::NotRunning)
      {
        qApp->processEvents();
         nMilliseconds = rtimer.elapsed();
       if (nMilliseconds >= 5000)
           break;
    }


    }


    /////////////////////////////////////////////////////
    bool MainWindow::mount_system(QString mnt)
    {



        QString cstring;
        QString command;


        cstring = getadb() + " shell /data/local/tmp/adblink/which su";
        command=getadbOutput(cstring);



          cstring = getadb() + " shell su -c /data/local/tmp/adblink/mount  -o "+ mnt + ",remount /";

          command=getadbOutput(cstring);



            if (command.isEmpty())
              {

                return true;
              }
                else
              {

                return false;
              }

    }



    /////////////////////////////
    bool MainWindow::is_su()
    {

    QString cstring = getadb() + " shell /data/local/tmp/adblink/which su";
    QString command=getadbOutput(cstring);



    if (!command.contains("su"))
      {

        return false;
       }
        else
      {
         return true;
       }


    }



    ////////////////////////////////////////////////
     QString MainWindow::strip (QString str)
    {
        str = str.simplified();
        str.replace( " ", "" );
        return str;
    }



     //////////////////////////////////////////
    void MainWindow::kill_server()
    {

     QString cstring = getadbpath() + " kill-server";
     QString command=getadbOutput(cstring);
     logfile("server stopped");

    }


    //////////////////////////////////////
    bool MainWindow::start_server()
    {

     bool serverRunning;

        QString cstring = getadbpath() + " kill-server";
        QString command=getadbOutput(cstring);


           cstring = getadbpath() + " start-server";
          command=getadbOutput(cstring);



         if (command.contains("daemon started successfully"))
            {

               serverRunning = true;
              }

             else
            {
             logfile("start-server failed!");
             logfile(cstring);
             logfile(command);
             serverRunning = false;
             }


             return serverRunning;

    }



    /////////////////////////////////////////////////////
    bool MainWindow::is_package(QString package)
    {


             bool is_packageInstalled;


        QString cstring = getadb() + " shell pm list packages ";


        QString command=getadbOutput(cstring);
        logfile ("package: "+cstring);

            if (command.contains(package))
                {
                logfile(package+ " is installed");
                is_packageInstalled = true;
                }
                else
                {
                logfile(package+ " not found");
                is_packageInstalled = false;}

            return  is_packageInstalled;
    }




    //////////////////////////////////

    bool MainWindow::isConnectedToNetwork()
    {
        QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
        bool result = false;

        for (int i = 0; i < ifaces.count(); i++)
        {
            QNetworkInterface iface = ifaces.at(i);
            if ( iface.flags().testFlag(QNetworkInterface::IsUp)
                 && !iface.flags().testFlag(QNetworkInterface::IsLoopBack) )
            {


                // this loop is important
                for (int j=0; j<iface.addressEntries().count(); j++)
                {

                    // we have an interface that is up, and has an ip address
                    // therefore the link is present

                    // we will only enable this check on first positive,
                    // all later results are incorrect

                    if (result == false)
                        result = true;
                }
            }

        }

        return result;
    }




    /////////////////////////////////////

    void MainWindow::delay(int secs)
    {
        QTime dieTime= QTime::currentTime().addSecs(secs);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }




////////////////////////////////////////////

    void MainWindow::do_versioncheck()
    {
        QJsonObject obj;
        QJsonDocument doc(obj);
        QFile file(databasedir + "adblink.json");
        if (file.open(QIODevice::ReadOnly)) {
            doc = QJsonDocument::fromJson(file.readAll());
            obj = doc.object();
            file.close();
        }
        bool checkversion = obj["checkversion"].toBool();
        bool startview = obj["startview"].toBool();

        if (startview) {
            ui->stackedWidget->setCurrentIndex(0);
            ui->menuKodi->menuAction()->setVisible(true);
            ui->infoArchitecture->setEnabled(true);
            ui->infoArchitecture->setVisible(true);

        } else {
            ui->stackedWidget->setCurrentIndex(1);
            ui->menuKodi->menuAction()->setVisible(false);
            ui->infoArchitecture->setEnabled(false);
            ui->infoArchitecture->setVisible(false);
        }

        if (checkversion) {
            if (!QUrl(vqurl).isValid()) {
                QMessageBox::critical(this, "", "Invalid URL for version check", QMessageBox::Cancel);
                return;
            }
            QNetworkRequest request;
            request.setUrl(QUrl(vqurl));
            QNetworkReply *reply = m_networkManager->get(request);
            connect(reply, &QNetworkReply::finished, this, &MainWindow::onReqCompleted);
        }
    }

    void MainWindow::onReqCompleted()
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
        if (!reply) {
            return;
        }

        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::critical(this, "", "Network error: " + reply->errorString(), QMessageBox::Cancel);
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QString s1(data);
        s1 = s1.trimmed();

        if (version != s1) {
            QDialog dialog(this);
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

            connect(&yesButton, &QPushButton::clicked, [&dialog]() {
                QDesktopServices::openUrl(QUrl("http://www.jocala.com"));
                dialog.close();
            });

            connect(&noButton, &QPushButton::clicked, [&dialog]() {
                dialog.close();
            });

            connect(&changelogButton, &QPushButton::clicked, this, &MainWindow::on_actionView_Changelog_triggered);

            dialog.exec();
        }

        reply->deleteLater();
    }





    //////////////////////////////////////////////////////////////////////
    void MainWindow::on_actionQuit_triggered()
    {

        QCoreApplication::quit();

    }





    ////////////////////////////////////////////////////////////////////////////
    bool MainWindow::installAPK(QString filename)
    {


        QString command;
        QString cstring;
        QString port;
        QString daddr;
        QString selectedDescription = ui->deviceTable->item(ui->deviceTable->currentRow(), 0)->text();




        DeviceRecord device = queryDeviceRecord(selectedDescription);




        logfile("Installing "+filename);


         cstring = getadb() + " install -r " + '"'+ filename+'"';

        command=RunLongProcess(cstring,"installing apk(s)");
        logfile(cstring);
        logfile(command);



        if (!command.contains("uccess") || command.contains("Failure"))
        {
            QMessageBox::critical(this,"",filename+" install failed.\nSee log.");
            return false;
        }
        else return true;




    }

    ////////////////////////////////////////////////////////////////////////////
    void MainWindow::sideload_Button_clicked()
    {





        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
            return;
        }



        DeviceRecord device = queryDeviceRecord(selectedDescription);


         bool installer=false;


         QString install = readInstall(databasedir);

         QStringList filenames = QFileDialog::getOpenFileNames(this, tr("APK files (*.apk);;All files (.*)"), install);



        if( !filenames.isEmpty() )
        {

            QMessageBox::StandardButton reply;
              reply = QMessageBox::question(this, "Install", "Install APKs?",
                                            QMessageBox::Yes|QMessageBox::No);
              if (reply == QMessageBox::Yes)
              {

                logfile("starting APK installation(s)");
                for (int i =0;i<filenames.count();i++)
                  installer = installAPK(filenames.at(i));

              }



          //    getadbpath() install --bypass-low-target-sdk-block someapp.apk

              if (installer)
              {


                  writeInstall(filenames[0].left(filenames[0].lastIndexOf('/')));
                  QMessageBox::information(this,"","APK(s) installed.\nSee log for details.");

                  install = filenames[0].left(filenames[0].lastIndexOf('/'));
                  writeInstall(install);


              }


        }


    }


    ///////////////////////////////////////////////////////////////////////////
    void MainWindow::uninstall_Button_clicked()
    {


        QString port;
        QString daddr;
        QString package = "";
        QString cstring;
        QString command;
        bool keepbox = false;

        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
              return;
        }


        DeviceRecord device = queryDeviceRecord(selectedDescription);






        logfile("open uninstall dialog");




        uninstallDialog dialog(device.daddr,port,this);
        dialog.setWindowModality(Qt::WindowModal);

        // dialog.setModal(true);
        if(dialog.exec() == QDialog::Accepted)
        {

        package = dialog.packageName();
        keepbox = dialog.keepBox();

        }

        else return;

        qDebug() << package;

        if (package.isEmpty())
           {
            QMessageBox::critical(this,"","No file selected");



            return;
            }



                if ( !is_package(package))
                   { QMessageBox::critical(
                         this,
                         "",
                         package +" not installed");
                      return;

                logfile("Error: "+ package +" not installed");
                }


                QMessageBox::StandardButton reply;
                      reply = QMessageBox::question(this, "Uninstall", "Uninstall "+package+"?",
                         QMessageBox::Yes|QMessageBox::No);
                      if (reply == QMessageBox::Yes) {



                         daddr=daddr+":"+port;

                          if (!keepbox)
                             cstring = getadb() + " shell pm uninstall " + package;
                          else
                             cstring = getadb() + " shell pm uninstall -k " + package;


                          logfile("uninstall: "+cstring);

                          QString command=RunLongProcess(cstring,"Uninstall APK");



                          if (!command.contains("Success"))
                              {
                               QMessageBox::critical(this,"","Uninstall failed");
                                logfile(package+" uninstalled");
                                }
                            else {
                               QMessageBox::information(this,"","Uninstalled");
                                logfile(package+" uninstalled");
                               }
         }
    }


//////////////////////////////////////////////


    void MainWindow::connButton_clicked()
    {
                      QString adhoc="Ad hoc";
                      QString cstring;
                      QString command;
                      QString s;
                      QString selectedDescription;
                      QString daddr;
                      QString port;

                      int selectedRow;

                      if (!ui->adhocip->text().isEmpty())
                      {
                            adhocip();
                            ui->adhocip->clear();
                            for (int row = 0; row < ui->deviceTable->rowCount(); ++row) {
                              QTableWidgetItem* item = ui->deviceTable->item(row, 0);
                              if (item && item->text() == adhoc) {
                  ui->deviceTable->selectRow(row);
                  break;
                              }
                            }
                      }

                      selectedRow = ui->deviceTable->currentRow();
                      if (selectedRow >= 0 && ui->deviceTable->item(selectedRow, 0)) {
                            selectedDescription = ui->deviceTable->item(selectedRow, 0)->text();
                      } else {
                            QMessageBox::critical(this, "", "No device selected in table");
                            return;
                      }

                      DeviceRecord device = queryDeviceRecord(selectedDescription);

                      if (device.isusb) {
                            logfile("USB connection attempted, not supported");
                            QMessageBox::critical(this, "", "Inactive for USB connections");
                            return;
                      }

                      if (!validateIPAddress(device.daddr)) {
                            QMessageBox::critical(this, "Error", "Invalid IP address");
                            return;
                      }

                      port = device.port.isEmpty() ? "5555" : device.port;
                      daddr = device.daddr + ":" + port;

                      cstring = getadbpath() + " connect " + daddr;
                      command = connectadb(cstring);

                      if (command.contains("failed to authenticate") || command.contains("offline")) {

                            ui->deviceTable->setItem(selectedRow, 2, new QTableWidgetItem(
                                                                         command.contains("failed to authenticate") ? "Unauthorized" : "Offline"));
                            logfile(cstring);
                            logfile(command);
                            QString cstring = getadbpath() + " disconnect " + daddr;
                            command = connectadb(cstring);
                            return;
                      }

                      logfile(cstring);
                      logfile(command);

                      if (command.contains("connected to")) {

                            ui->deviceTable->setItem(selectedRow, 2, new QTableWidgetItem("Connected"));

                            ui->deviceTable->clearSelection();
                            ui->deviceTable->setCurrentCell(selectedRow, 0);
                            ui->deviceTable->selectRow(selectedRow);
                            ui->deviceTable->setFocus();

                            logfile("Connected to " + daddr);
                            logfile("Android version: " + s.setNum(getandroid()));
                      } else {

                            ui->deviceTable->setItem(selectedRow, 2, new QTableWidgetItem("NA"));
                            logfile("Unable to connect to: " + daddr);
                            QMessageBox::critical(this, "", "Unable to connect to: " + daddr);
                      }


    }


////////////////////////////////////////////

    void MainWindow::adhocip()
    {
                      QString cstring;
                      QString command;
                      QString daddr;
                      QString port;

                      if (!ui->adhocip->text().isEmpty())
                      {
                            QString adhocIPText = ui->adhocip->text().trimmed();
                            int colonIndex = adhocIPText.indexOf(':');
                            QString daddr, port;

                            if (colonIndex != -1) {
                              daddr = adhocIPText.left(colonIndex).trimmed();
                              port = adhocIPText.mid(colonIndex + 1).trimmed();
                            } else {
                              daddr = adhocIPText;
                              port = "5555";
                            }


                            bool ok;
                            int portNum = port.toInt(&ok);
                            if (!ok || portNum < 1 || portNum > 65535) {
                              logfile("Invalid port: " + port);
                              QMessageBox::critical(this, "", "Invalid port: " + port);
                              return;
                        }

                            QSqlQuery query;
                            query.prepare("INSERT OR REPLACE INTO device (description, daddr, port, isusb, data_root, xbmcpackage, filepath) "
                                          "VALUES (:description, :daddr, :port, :isusb, :data_root, :xbmcpackage, :filepath)");
                            query.bindValue(":description", "Ad hoc");
                            query.bindValue(":daddr", daddr);
                            query.bindValue(":port", port);
                            query.bindValue(":isusb", 0);
                            query.bindValue(":data_root", "/sdcard/");
                            query.bindValue(":xbmcpackage", "org.xbmc.kodi");
                            query.bindValue(":filepath", "/files/.kodi");
                            if (!query.exec()) {
                              logfile("Failed to insert temporary device: " + query.lastError().text());
                            } else {
                              logfile("Temporary device record inserted: Ad hoc IP, " + daddr + ":" + port);
                            }
                      }

                      loadDeviceTable();
    }

    ////////////////////////////////////////////////////////////////

    void MainWindow::disButton_clicked()
    {
             QString daddr;
             int selectedRow = ui->deviceTable->currentRow();

             // Validate selection and connection status
             if (selectedRow < 0 || !ui->deviceTable->item(selectedRow, 2) || !ui->deviceTable->item(selectedRow, 1)) {
            QMessageBox::critical(this, "", "No valid device selected");
            return;
             }
             if (ui->deviceTable->item(selectedRow, 2)->text() != "Connected") {
            QMessageBox::critical(this, "", "Selected device is not connected");
            return;
             }

             if (ui->deviceTable->item(selectedRow, 1)->text().contains("USB")) {
            QMessageBox::critical(this, "", "Inactive for USB connections");
            return;
             }



             daddr = ui->deviceTable->item(selectedRow, 1)->text();



             // Confirm disconnection
             QMessageBox::StandardButton reply;
             reply = QMessageBox::question(this, "Disconnect", "Disconnect device?",
                                           QMessageBox::Yes | QMessageBox::No);
             if (reply == QMessageBox::No) {
            return;
             }

             // Execute disconnect command
             QString cstring = getadbpath() + " disconnect " + daddr;
             QString command = getadbOutput(cstring);
             logfile(command);
             logfile("disconnect: " + daddr);

             // Update status column (column 2) to "Disconnected"
             if (selectedRow >= 0 && ui->deviceTable->item(selectedRow, 2)) {
            ui->deviceTable->setItem(selectedRow, 2, new QTableWidgetItem("Disconnected"));
             }


    }




    /////////////////////////////////////////////////////////////////////////
    void MainWindow::on_actionAbout_triggered()
    {
             // Read donation value from adblink.json
             QString donation;
             QFile file(databasedir + "adblink.json");
             if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            if (!doc.isNull()) {
                              QJsonObject obj = doc.object();
                              donation = obj["donation"].toString();
            } else {
                              qDebug() << "Error: Invalid JSON in adblink.json";
            }
            file.close();
             } else {
            qDebug() << "Error: Could not open adblink.json at" << databasedir;
             }

             // Create Dialog2 and pass the donation value
             Dialog2 dialog2(this, donation);
             dialog2.setWindowModality(Qt::WindowModal);
             dialog2.setaLabel(program + " " + version);
             dialog2.exec();
    }

    //////////////////////////////////////////////
    void MainWindow::on_actionHelp_triggered()
    {

        helpDialog helpdialog;


        helpdialog.setWindowModality(Qt::NonModal);
        helpdialog.setWindowTitle("Help");
        helpdialog.exec();
    }



    void MainWindow::on_actionPush_remote_triggered()
    {

    QString cstring;
    QString command;
    QString mcpath="";

    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
               return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);


    if (!is_package(device.xbmcpackage))
    { QMessageBox::critical(
                   this,
                   "",
                   device.xbmcpackage+" not installed");
               return;
    }




    cstring = getadb() + " shell ls /sdcard/xbmc_env.properties";
    if(getreturncode(cstring))
    {  cstring = getadb() + " shell cat /sdcard/xbmc_env.properties";
               command=getadbOutput(cstring);
               command.replace(QRegExp("[\r\n]"), "");
               mcpath = command.mid(command.indexOf("xbmc.data=") + 10);
               mcpath=mcpath+"/.kodi";
    }
    else

    {
               mcpath="/sdcard/Android/data/" + device.xbmcpackage+"/files/.kodi";

    }




    cstring = getadb() + " shell ls "+mcpath;

    command=getadbOutput(cstring);

    if (command.contains("No such file or directory"))
    {
               QMessageBox::critical(
                   this,
                   "",
                   "Kodi data not found"+mcpath);
               return;
    }

    mcpath = mcpath+"/userdata/keymaps/";
    cstring = getadb() + " shell ls "+mcpath;

    command=getadbOutput(cstring);

    if (command.contains("No such file or directory"))
    {
               cstring = getadb() + " mkdir -p "+ mcpath;
               command=getadbOutput(cstring);
    }




    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Choose remote xml file", QDir::homePath(), tr("Files (*.xml)"));

    if (!fileName.isEmpty() )
    {


               QMessageBox::StandardButton reply;
               reply = QMessageBox::question(this, "Push", fileName+" selected. Continue?",QMessageBox::Yes|QMessageBox::No);

               if (reply == QMessageBox::No)
                   return;


               cstring = getadb() + " push "+'"'+fileName+'"'+ " "+mcpath+"/keyboard.xml";
               command=getadbOutput(cstring);





               logfile("push remote:"+command);



               if (command.contains("bytes"))


               {


                   QMessageBox::information(this,"","Remote xml installed.");


               }
               else

               {

                   QMessageBox::critical(
                       this,
                       "",
                       "Remote xml installation failed.");}



    }





    }







    ////////////////////////////////////////////////////////////////////////////////
    void MainWindow::on_actionReboot_triggered()
    {


          QString selectedDescription;
          if (!validateDeviceSelection(selectedDescription)) {
           return;
          }

          DeviceRecord device = queryDeviceRecord(selectedDescription);





       QMessageBox::StandardButton reply;
         reply = QMessageBox::question(this, "Reboot Device", "Reboot Device?",
             QMessageBox::Yes|QMessageBox::No);
         if (reply == QMessageBox::Yes) {
            logfile("rebooting device");
            rebootDevice(" reboot");
         }

    }


    /////////////////////////////////////////
    void MainWindow::on_donate_clicked()
    {

            qDebug() << "xxxx";

       QString link = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=GKZMW456H6E5W";
       QDesktopServices::openUrl(QUrl(link));

    }

////////////////////////////////////////

    void MainWindow::killServer_clicked()
    {
       if (QMessageBox::question(this, "Disconnect", "Disconnect all IPs?", QMessageBox::Cancel | QMessageBox::Ok) == QMessageBox::Cancel) {
            return;
       }

       // Kill the ADB server
       QString cstring = getadbpath() + " kill-server";
       QString command = getadbOutput(cstring);


       // Update device table
       for (int row = 0; row < ui->deviceTable->rowCount(); ++row) {
            QTableWidgetItem* descItem = ui->deviceTable->item(row, 0);
            if (!descItem) continue; // Skip if no item

            // Get the current status from column 2
            QTableWidgetItem* statusItem = ui->deviceTable->item(row, 2);
            QString currentStatus = statusItem ? statusItem->text() : "";

            // Check if the device is USB based on stored data
            bool isUsb = descItem->data(Qt::UserRole + 1).toBool();

            // Only update status to "Disconnected" if it's not already "USB"
            if (currentStatus != "USB") {
                   QString status = isUsb ? "USB" : "Disconnected";
                   QTableWidgetItem* newStatusItem = new QTableWidgetItem(status);
                   ui->deviceTable->setItem(row, 2, newStatusItem);
            }
       }

       ui->deviceTable->viewport()->update();
    }


//////////////////

    void MainWindow::dataentry(bool isNewRecord)
    {
             QString cstring;
             QString command;
             QStringList mstringlist;
             QStringList dstringlist;
             QString selectedDescription;
             DeviceRecord device;

             cstring = getadbpath() + " devices";
             command = getadbOutput(cstring);
             QThread::sleep(2);

             mstringlist = command.split(QRegExp("[\t\n\r]"), QString::SkipEmptyParts);

             if (command.contains("List of devices attached"))
             {
             mstringlist.removeFirst();
             for (int a = 0; a < mstringlist.size(); a = a + 2)
             {
                   QStringList pieces = mstringlist.at(a).split(":", QString::SkipEmptyParts);
                   if (!mstringlist.at(a).contains("daemon"))
                   dstringlist << pieces.at(0);
             }
             }

             // For update, get selected description from deviceTable
             if (!isNewRecord)
             {
             int selectedRow = ui->deviceTable->currentRow();
             if (selectedRow >= 0 && ui->deviceTable->item(selectedRow, 0))
             {
                   selectedDescription = ui->deviceTable->item(selectedRow, 0)->text();
                   device = queryDeviceRecord(selectedDescription);                 
             }
             else
             {
                   QMessageBox::critical(this, "", "No device selected in table");
                   return;
             }
             }

             bool iskodi;

             if (ui->stackedWidget->currentIndex() == 0)
             iskodi = true;
             else
             iskodi = false;

             preferencesDialog dialog(this, iskodi);

             dialog.setWindowModality(Qt::WindowModal);

             dialog.setversionLabel(version);
             dialog.setdevicelist(dstringlist);

             // Initialize dialog fields based on insert or update
             if (isNewRecord)
             {
             // Default values for new record (insert)
             dialog.setPackagename("org.xbmc.kodi");
             dialog.setPulldir("");
             dialog.setfilepath("/files/.kodi");
             dialog.setdataroot("/sdcard/");
             dialog.setostype("0");
             dialog.setdescription("");
             dialog.setdisableroot(0);
             dialog.setport("5555");
             dialog.setscope(false);
             dialog.setwsa(false);
             dialog.setdaddr("");
             dialog.setisusb(false);
             }
             else
             {
             // Populate with database values for update
             dialog.setPackagename(device.xbmcpackage);
             dialog.setPulldir(device.pulldir);
             dialog.setfilepath(device.filepath);
             dialog.setdataroot(device.data_root);
             dialog.setostype("0");
             dialog.setdescription(device.description);
             dialog.setdisableroot(device.disableroot);
             if (device.isusb)
                   dialog.setport("");
             else
             dialog.setport(device.port);
             dialog.setdaddr(device.daddr);
             dialog.setisusb(device.isusb);
             }




             dialog.setModal(true);

             int result = dialog.exec();
             if (result == QDialog::Accepted)
             {
             // Retrieve values from dialog
             QString data_root = dialog.data_root();
             QString xbmcpackage = dialog.xbmcpackageName();
             QString pulldir = dialog.pulldir();
             QString description = dialog.description();
             QString filepath = dialog.filepath();
             QString port = dialog.port();
             QString daddr = dialog.daddr();
             bool isusb = dialog.isusb();
             QString ostype = dialog.ostype();
             int disableroot = dialog.disableroot();

             // Validate description (non-empty)
             if (description.isEmpty())
             {
                   QMessageBox::critical(this, "", "Description cannot be empty.");
                   return;
             }

             // Check for duplicate description during insert
             if (isNewRecord)
             {
                   QSqlQuery checkQuery;
                   checkQuery.prepare("SELECT COUNT(*) FROM device WHERE description = ?");
                   checkQuery.addBindValue(description);
                   if (checkQuery.exec() && checkQuery.first() && checkQuery.value(0).toInt() > 0)
                   {
                   QMessageBox::critical(this, "", "A device with this description already exists.");
                   return;
                   }
             }

             QSqlQuery query;
             QString sqlstatement;

             if (isNewRecord)
             {
                   // Insert new record
                   sqlstatement = "INSERT INTO device (description, daddr, port, isusb, ostype, "
                                  "data_root, xbmcpackage, pulldir, disableroot, filepath) "
                                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
                   query.prepare(sqlstatement);
                   query.addBindValue(description);
                   query.addBindValue(daddr);
                   query.addBindValue(port);
                   query.addBindValue(isusb);
                   query.addBindValue(ostype);
                   query.addBindValue(data_root);
                   query.addBindValue(xbmcpackage);
                   query.addBindValue(pulldir);
                   query.addBindValue(disableroot);
                   query.addBindValue(filepath);
             }
             else
             {
                   // Update existing record
                   sqlstatement = "UPDATE device SET description = ?, daddr = ?, port = ?, isusb = ?, ostype = ?, "
                                  "data_root = ?, xbmcpackage = ?, pulldir = ?, disableroot = ?, filepath = ? "
                                  "WHERE description = ?";
                   query.prepare(sqlstatement);
                   query.addBindValue(description);
                   query.addBindValue(daddr);
                   query.addBindValue(port);
                   query.addBindValue(isusb);
                   query.addBindValue(ostype);
                   query.addBindValue(data_root);
                   query.addBindValue(xbmcpackage);
                   query.addBindValue(pulldir);
                   query.addBindValue(disableroot);
                   query.addBindValue(filepath);
                   query.addBindValue(selectedDescription);
             }

             if (!query.exec())
             {
                   logfile("Query error: " + query.lastError().text());
                   QMessageBox::critical(this, "", (isNewRecord ? "Failed to insert into database: " : "Failed to update database: ") + query.lastError().text());
                   return;
             }


             }


             QSqlDatabase::database().commit();
             loadDeviceTable();

    }



    ///////////////////////////////////////////
    void MainWindow::cacheButton_clicked()

    {



    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
              return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);


    QString buffersize = "";
    QString bufferfactor = "";
    int buffermode = 1;


    bool oldxml = false;


    QString line1;
    QString line2;
    QString line3;
    QString line4;
    QString line5;
    QString line6;
    QString line7;



    QString cstring;
    QString command;
    QString hidden;
    QString xpath = "";
    QString mcpath="";



    cstring = getadb() + " shell ls /sdcard/xbmc_env.properties";
    if(getreturncode(cstring))
    {  cstring = getadb() + " shell cat /sdcard/xbmc_env.properties";
              command=getadbOutput(cstring);
              command.replace(QRegExp("[\r\n]"), "");
              mcpath = command.mid(command.indexOf("xbmc.data=") + 10);
              mcpath=mcpath+"/.kodi";
    }
    else

    {
              if (isScoped())
                   mcpath=device.data_root + "kodi_data/" + device.xbmcpackage+"/files/.kodi";
              else
                   mcpath=device.data_root + "Android/data/" + device.xbmcpackage+"/files/.kodi";

    }







    xpath = mcpath+"/userdata/";

    cstring = getadb() + " shell ls "+mcpath;

    command=getadbOutput(cstring);

    if (command.contains("No such file or directory"))
    {
              QMessageBox::critical(
                  this,
                  "",
                  "Cache folder missing. Run Kodi to create it.");
              return;
    }

    cacheDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setxmlcheck(false);
    dialog.setcbuffermode(buffermode);
    dialog.setcbuffersize(buffersize);
    dialog.setcbufferfactor(bufferfactor);

    dialog.setModal(true);


    if(dialog.exec() == QDialog::Accepted)
    {

              int x = dialog.returncval1();


              buffermode = dialog.cbuffermode();
              buffersize = dialog.cbuffersize();
              bufferfactor = dialog.cbufferfactor();
              oldxml = dialog.xmlcheck();

              QString str1;
              str1.setNum(buffermode-1);




              if (oldxml)
              {
                   line1 ="<advancedsettings>";
                   line2 ="  <network>";
                   line3 ="    <buffermode>"+str1+"</buffermode>";
                   line4 ="    <cachemembuffersize>"+buffersize+"</cachemembuffersize>";
                   line5 ="    <readbufferfactor>"+bufferfactor+"</readbufferfactor>";
                   line6 ="  </network>";
                   line7 ="</advancedsettings>";
              }

              else

              {
                   line1 ="<advancedsettings>";
                   line2 ="  <cache>";
                   line3 ="    <buffermode>"+str1+"</buffermode>";
                   line4 ="    <memorysize>"+buffersize+"</memorysize>";
                   line5 ="    <readfactor>"+bufferfactor+"</readfactor>";
                   line6 ="  </cache>";
                   line7 ="</advancedsettings>";
              }



              if (x == 2)
              {


                   logfile("write advancedsettings.xml");



                   cstring = getadb() + " shell ls "+mcpath;


                   QString filename1 = "advancedsettings.xml";
                   QString filename2 = apphome+filename1;



                   cstring = getadb() + " shell ls "+mcpath;
                   command=getadbOutput(cstring);
                   if (command.contains("No such file or directory"))
                   {
                        QMessageBox::critical(this,"","Destination path missing");
                        return;
                   }



                   cstring = getadb() + " shell ls "+xpath+filename1;
                   command=getadbOutput(cstring);

                   // logfile(cstring);
                   logfile(command);

                   if (!command.contains("No such file or directory"))
                   {
                        logfile("advancedsettings.xml exists");

                        QMessageBox::StandardButton reply;
                        reply = QMessageBox::question(this, "XML", "advancedsettings.xml exists. Overwrite?\n(will backup original)",
                                                      QMessageBox::Yes|QMessageBox::No);
                        if (reply == QMessageBox::No) {
                            logfile("abort xml write");
                            return;
                        } else {
                            logfile("continue xml write");
                            logfile("backup advancedsettings.xml");
                            cstring = getadb() + " shell cp "+xpath+filename1+" "+xpath+filename1+".old";
                            command=getadbOutput(cstring);
                        }
                   }  // end if exists


                   QFile file(filename2);


                   if(!file.open(QFile::WriteOnly))
                   {
                        logfile("error creating advancedsettings.xml.");
                        QMessageBox::critical(this,"","Unknown error creating xml file!");
                        return;
                   }

                   QTextStream out(&file);

                   out  << line1 << endl;
                   out  << line2 << endl;
                   out  << line3 << endl;
                   out  << line4 << endl;
                   out  << line5 << endl;
                   out  << line6 << endl;
                   out  << line7 << endl;

                   file.flush();
                   file.close();

                   cstring = getadb() + " push "+filename2+ " "+xpath+filename1;
                   command=getadbOutput(cstring);

                   if (!command.contains("bytes"))
                   {
                        logfile(command);
                        logfile("error pushing xml script to device!");
                        QMessageBox::critical(this,"","Error pushing xml from PC to device!");
                        return;
                   }
                   else {
                        QMessageBox::information(this,"","advancedsettings.xml written");
                   }



              }


    }



    }


    //////////////////////////////////////////////////
    void MainWindow::on_actionView_Kodi_Log_triggered()
    {

    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
               return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);

            androidLog();

    }

    //////////////////////////////////////////////////
    void MainWindow::androidLog()
    {
          QString selectedDescription;
          if (!validateDeviceSelection(selectedDescription)) {
            return;
          }

          DeviceRecord device = queryDeviceRecord(selectedDescription);



          QString xpath = "";
          QString cstring;
          QString command;
          QString mcpath="";



          cstring = getadb() + " shell ls /sdcard/xbmc_env.properties";
          if(getreturncode(cstring))
          {  cstring = getadb() + " shell cat /sdcard/xbmc_env.properties";
            command=getadbOutput(cstring);
            command.replace(QRegExp("[\r\n]"), "");
            mcpath = command.mid(command.indexOf("xbmc.data=") + 10);
            mcpath=mcpath+"/.kodi";
          }
          else

          {
            if (isScoped())
                  mcpath=device.data_root + "kodi_data/" + device.xbmcpackage+"/files/.kodi";
            else
                  mcpath=device.data_root + "Android/data/" + device.xbmcpackage+"/files/.kodi";

          }





          xpath = mcpath+"/temp/";


          qDebug() << xpath;

          cstring = getadb() + " shell "+busypath+"busybox find " +xpath+ " -maxdepth 1 -name kodi.log ";

          command=getadbOutput(cstring);


          if (command.isEmpty() || command.contains("No such file or directory"))
          { QMessageBox::critical(this,"","Kodi log not found");

            // logfile(cstring);
            logfile(command);
            logfile("Kodi log not found!");
            return;
          }






        device.filepath=xpath;

        logfile("opening kodi log");
        klogDialog klogdialog;
        klogdialog.passdata(getadb(),device.data_root,device.filepath,device.xbmcpackage);
        klogdialog.setModal(true);
        klogdialog.exec();

    }

    ////////////////////////////////////////////////////
    void MainWindow::on_actionView_adbLink_Log_triggered()
    {
        adblogDialog dialog;
        dialog.setModal(true);
        dialog.exec();

    }



    ///////////////////////////////////////////////
    bool MainWindow::is_busybox()
    {

    QString cstring;
    QString command;


    cstring = getadb() + " shell ls /data/local/tmp/adblink";
    command=getadbOutput(cstring);


    if (command.contains("No such file or directory"))
       {
        cstring = getadb() + " shell mkdir -p /data/local/tmp/adblink";
        command=getadbOutput(cstring);
       }

      cstring = getadb() + " shell ls /data/local/tmp/adblink/busybox";
      command=getadbOutput(cstring);


      if (command.contains("No such file or directory"))
         {

      QString busybox = '"' + QCoreApplication::applicationDirPath() + "/adbfiles/busybox" + '"';

          cstring = getadb() + " push "+busybox+ " /data/local/tmp/adblink/";

          QString command=getadbOutput(cstring);
          if (!command.contains("bytes"))
            {
               logfile("busybox install failed ");
               logfile(command);
               
               QMessageBox::critical(0,"","busybox install failed. See log.");
               return false;
              }
         else
          {


              logfile(command);

              cstring = getadb() + " shell chmod 755 /data/local/tmp/adblink/busybox";
              command=getadbOutput(cstring);

              // logfile(cstring);
              logfile(command);

               cstring = getadb() + " shell /data/local/tmp/adblink/busybox --install -s /data/local/tmp/adblink";
               command=getadbOutput(cstring);

               // logfile(cstring);
               logfile(command);

          }


      }

      return true;

    }



    //////////////////////////////////////////////////////////
    void MainWindow::on_actionPaste_path_triggered()
    {

        QClipboard *pathClipboard = QApplication::clipboard();
        pathClipboard->setText("PATH=$PATH:/data/local/tmp/adblink\n");
    }


    //////////////////////////////////////////////////////////
    void MainWindow::usbbuttons(bool isusb)
    {

     return;

        if(!isusb)
               {
                  ui->connButton->setDisabled(false);
                  ui->disButton->setDisabled(false);
                  }
              else
                  {
                    ui->connButton->setDisabled(true);
                    ui->disButton->setDisabled(true);
                  }

    }


    //////////////////////////////////////////////////////

    void MainWindow::on_actionStop_Application_triggered()
    {
                  QString selectedDescription;
                  if (!validateDeviceSelection(selectedDescription)) {
                    return;
                  }

                  DeviceRecord device = queryDeviceRecord(selectedDescription);




          bool startstop;

          QString stopapp;

          if (QFileInfo::exists(databasedir+"/stopapp.json"))
                 startstop = true;
          else
                 startstop = false;

          if (!startstop)
          {



                 QJsonObject obj;
                 obj["stopapp"] = "org.xbmc.kodi";
                 QJsonDocument doc(obj);
                 QFile file(databasedir+"stopapp.json");
                 file.open(QIODevice::WriteOnly);
                 file.write(doc.toJson());
                 file.close();


          }



          QJsonObject obj;
          QJsonDocument doc(obj);
          QFile file(databasedir+"stopapp.json");
          file.open(QIODevice::ReadOnly);
          doc = QJsonDocument::fromJson(file.readAll());
          obj = doc.object();
          stopapp=obj["stopapp"].toString();
          file.close();


        forcequitDialog dialog(false,stopapp,this);
        dialog.setWindowModality(Qt::WindowModal);

        if(dialog.exec() == QDialog::Accepted)
        {


        QString cstring = getadb() + " shell am force-stop "+dialog.packagename();
        QString command=getadbOutput(cstring);
         logfile(cstring);
         logfile(command);

        QJsonObject obj;
        obj["stopapp"] = dialog.packagename();
        QJsonDocument doc(obj);
        QFile file(databasedir+"stopapp.json");
        file.open(QIODevice::WriteOnly);
        file.write(doc.toJson());
        file.close();

        }


    }

    /////////////////////////////////////////////////////////

    void MainWindow::on_actionStart_Application_triggered()
    {

        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
        return;
        }

        DeviceRecord device = queryDeviceRecord(selectedDescription);




                bool startstop;
                QString startapp;


                if (QFileInfo::exists(databasedir+"/startapp.json"))
                  startstop = true;
                else
                 startstop = false;

                if (!startstop)
                {
                  QJsonObject obj;
                  obj["startapp"] = "org.xbmc.kodi/org.xbmc.kodi.Splash";
                  QJsonDocument doc(obj);
                  QFile file(databasedir+"startapp.json");
                  file.open(QIODevice::WriteOnly);
                  file.write(doc.toJson());
                  file.close();
                }



                QJsonObject obj;
                QJsonDocument doc(obj);
                QFile file(databasedir+"startapp.json");
                file.open(QIODevice::ReadOnly);
                doc = QJsonDocument::fromJson(file.readAll());
                obj = doc.object();
                startapp=obj["startapp"].toString();
                file.close();



         forcequitDialog dialog(true,startapp, this);
         dialog.setWindowModality(Qt::WindowModal);


        if(dialog.exec() == QDialog::Accepted)
        {

            QString cstring = getadb() + " shell am start -n "+dialog.packagename();
            QString command=getadbOutput(cstring);
            logfile(cstring);
            logfile(command);

            QJsonObject obj;
            obj["startapp"] = dialog.packagename();
            QJsonDocument doc(obj);
            QFile file(databasedir+"startapp.json");
            file.open(QIODevice::WriteOnly);
            file.write(doc.toJson());
            file.close();

        }

    }


    ////////////////////////////////////////////////

    void MainWindow::keypadButton_clicked()
    {



        QString port;
        QString daddr;

        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
            return;
        }


        DeviceRecord device = queryDeviceRecord(selectedDescription);




        if (device.port.isEmpty()) {
            port = "5555";
        }
        else
            port=device.port;

        daddr = device.daddr+":"+port;

         QString cstring = daddr + " shell input keyevent ";
         keyboardDialog dialog(this);
         dialog.setWindowModality(Qt::WindowModal);
         dialog.setdaddr(cstring);
         dialog.exec();

    }


    ///////////////////////////////////////////////////////


    void MainWindow::fmButton_clicked()
    {

         QString selectedDescription;
         if (!validateDeviceSelection(selectedDescription)) {
            return;
         }

         DeviceRecord device = queryDeviceRecord(selectedDescription);




         QJsonObject obj;
         QJsonDocument doc(obj);
         QFile file(databasedir + "adblink.json");
         file.open(QIODevice::ReadOnly);
         doc = QJsonDocument::fromJson(file.readAll());
         obj = doc.object();
         QString download = doc.object()["download"].toString();
         QString cstring;
         QString command;
         QString mcpath;
         QString fmpullpath;
         QString pulldir;
         QString port;
         QString daddr;
         QString ostypefm("");
         QString fmdaddr("");
         bool iskodi;

         busybox_permissions();
         fmdaddr = device.daddr;



         if (device.isusb) {
          port = "";
          daddr = device.daddr;
         } else {
          port = device.port.isEmpty() ? "5555" : device.port;
          daddr = device.daddr + ":" + port;
         }


         if (ui->stackedWidget->currentIndex() == 0)
            iskodi=true;
         else
            iskodi=false;

         fmdialog = new usbfileDialog(iskodi,this);

         fmdialog->setWindowModality(Qt::NonModal);

         cstring = getadb() + " shell ls /sdcard/xbmc_env.properties";
         if (getreturncode(cstring))
         {
            cstring = getadb() + " shell cat /sdcard/xbmc_env.properties";
            command = getadbOutput(cstring);
            command.replace(QRegExp("[\r\n]"), "");
            mcpath = command.mid(command.indexOf("xbmc.data=") + 10);
            mcpath = mcpath + "/.kodi";
         }
         else
         {
            if (isScoped())
              mcpath = device.data_root + "kodi_data/" + device.xbmcpackage + "/files/.kodi";
            else
              mcpath = device.data_root + "Android/data/" + device.xbmcpackage + "/files/.kodi";
         }






         fmdialog->setkodiPath(mcpath);

         if (!ui->adhocip->text().isEmpty())
         {
            fmdialog->setData(ui->adhocip->text());
            fmdialog->setADB(getadbpath());
         }
         else
         {



            fmdialog->setADB(getadbpath() + " -s " + daddr);
            fmdialog->setData(selectedDescription);
         }




         QString kp = device.data_root;






         if (pulldir.isEmpty() || pulldir == download)
            fmpullpath = download;
         else
            fmpullpath = pulldir;

         QDir directory(fmpullpath);

         if (!directory.exists()) {
            logfile("Pull path: " + fmpullpath + " not found");
            logfile("Defaulting to home directory: " + QDir::homePath());
            fmpullpath = QDir::homePath();
         }

         fmdialog->setPath1("/sdcard/");
         fmdialog->setPath2("/sdcard/");
         fmdialog->setuProgram(kp);
         fmdialog->setPulldir(fmpullpath);
         fmdialog->setAdbdir(apphome);

         connect(fmdialog, &QDialog::finished, this, &MainWindow::handleFilemanagerFinished);

         QSettings settings("jocala", "adblink");

         QByteArray savedGeometry = settings.value("fmdialogGeometry").toByteArray();
         if (!savedGeometry.isEmpty()) {
            // qDebug() << "Restoring geometry";
            fmdialog->restoreGeometry(savedGeometry);
         } else {
            // qDebug() << "No saved geometry found";
         }

         fmdialog->show();
    }



/////////////////////////////////////////////////
 void MainWindow::handleFilemanagerFinished()
    {
        if (fmdialog) {
           QByteArray geometryData = fmdialog->saveGeometry();
           // qDebug() << "Geometry data:" << geometryData;
           QSettings settings("jocala", "adblink");
           settings.setValue("fmdialogGeometry", geometryData);
        }
    }

 ////////////////////////////////////////////////////////////


    QString MainWindow::RunLongProcess(QString cstring, QString jobname)
    {
        ui->progressBar->setHidden(false);
        ui->progressBar->setValue(0);
        QString command;
        QString s = jobname;
        RunProcessList << s;
        ui->server_running->setText(s);

        int tsvalue = 4000;

        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
        timer->start(tsvalue);

        command = getadbOutput(cstring);

        RunProcessList.removeAll(s);

        if (RunProcessList.count() > 0)
        {
           //activityIcon(true);
           ui->progressBar->setHidden(false);
           ui->progressBar->setValue(0);
        }
        else
        {
           //activityIcon(false);
           ui->progressBar->setHidden(true);
           ui->progressBar->setValue(0);
           ui->server_running->setText("");
        }

        serverlabel();

        return command;
    }

   ////////////////////////////////////////////////////////

    void MainWindow::on_actionMount_system_RO_triggered()
    {


            QString selectedDescription;
            if (!validateDeviceSelection(selectedDescription)) {
             return;
            }

            DeviceRecord device = queryDeviceRecord(selectedDescription);



        if (!is_su())
           {
             QMessageBox::critical(this,"","Root required!");
            return;
           }


        if (mount_system("ro"))
         {

            logfile("/system partition is read-only");

            QMessageBox::information(this,"","/ partition is read-only");
         }

         else

        {
            logfile("filesystem not remounted r/o!");

             QMessageBox::critical(this,"","filesystem not remounted r/o!");
        }


    }

    ////////////////////////////////////////////////////////

    void MainWindow::on_actionMount_system_RW_triggered()
    {

        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
             return;
        }

        DeviceRecord device = queryDeviceRecord(selectedDescription);





        if (!is_su())
        {
            QMessageBox::critical(this,"","Root required!");
        return;
        }



        if (mount_system("rw"))

           {

            logfile("/system partition is read-only");

            QMessageBox::information(this,"","/ partition is read-write");
        }
         else

        {
            logfile("filesystem not remounted r/o!");

                QMessageBox::critical(this,"","filesystem not mounted read-write");
        }


    }

    ///////////////////////////////////////////////////////

    void MainWindow::on_actionWireless_ADBD_triggered()
    {
        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
                return;
        }

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        QString port;
        QString daddr;


        if (device.isusb) {
                port = "";
                daddr = device.daddr;
        } else {
                port = device.port.isEmpty() ? "5555" : device.port;
                daddr = device.daddr + ":" + port;
        }





       tcpipDialog dialog;


       QString tcpstatus = getadb() +   " shell getprop persist.adb.tcp.port 5555";
           QString command=getadbOutput(tcpstatus);

           logfile(command);

           if (command.contains("5555"))
             dialog.settcplabel("ADB/WIFI is enabled");
           else
            dialog.settcplabel("ADB/WIFI is disabled");





           QString cstring = getadb() +  " tcpip "+port;
           command=getadbOutput(cstring);

            if(dialog.exec() == QDialog::Accepted)
             {
                QString command=getadbOutput(cstring);
                logfile(command);
              }
    }


    ////////////////////////////////////////////////////////

    void MainWindow::on_actionKodi_data_usage_triggered()
    {
              QString kodidata;



              QString selectedDescription;
              if (!validateDeviceSelection(selectedDescription)) {
                return;
              }

              DeviceRecord device = queryDeviceRecord(selectedDescription);

              backupDialog dialog;

             QString n_data_root;

              dialog.setadb_backup(getadb(),device.data_root);

               dialog.setModal(true);

               dialog.setWindowTitle("Kodi Data Size");

           if(dialog.exec() == QDialog::Accepted)
           {



               n_data_root = dialog.return_data_root();

                if(!n_data_root.startsWith("/"))
                   n_data_root.prepend("/");

                if(!n_data_root.endsWith("/"))
                   n_data_root.append("/") ;


                QString cstring = getadb() + " shell du -sh " + n_data_root +"Android/data/"+device.xbmcpackage;
                QString command=RunLongProcess(cstring,"calculating data size");


                if (command.contains("No such file"))
                {
                    kodidata = "No data found";
                }

                else
                {



                int z = command.indexOf("G");

                if (z==-1)
                    z = command.indexOf("M");

                if (z==-1)
                    z = command.indexOf("K");


                if (z != -1)
                 kodidata = command.mid(0,z+1);

                }



                QMessageBox::information(0,"Kodi Data","Kodi data size:  " + kodidata);





              }


    }

    ////////////////////////////////////////////////////////////////////////

    void MainWindow::on_clearAdhocButton_clicked()
    {
        ui->adhocip->setText("");


    }


////////////////////////////////////////////////////////////

    void MainWindow::on_actionPreferences_triggered()
    {
         adbprefDialog dialog(this);
         dialog.setWindowModality(Qt::WindowModal);
         QJsonObject obj;


         QJsonDocument doc(obj);
         QFile file(databasedir + "adblink.json");
         file.open(QIODevice::ReadOnly);
         doc = QJsonDocument::fromJson(file.readAll());
         obj = doc.object();

         QString dropdown = obj["dropdown"].toString();
         QString download = obj["download"].toString();
         QString install = obj["install"].toString();
         QString backup = obj["backup"].toString();
         QString donation = obj["donation"].toString();
         QString localadb = obj["localadb"].toString();


         bool checkversion = doc.object()["checkversion"].toBool();
         bool scrcpy = doc.object()["scrcpy"].toBool();
         bool startview = doc.object()["startview"].toBool();


         file.close();

         if (checkversion)
             dialog.setversioncheck(true);
         else
             dialog.setversioncheck(false);



         if (scrcpy)
             dialog.setscrcpyargs(true);
         else
             dialog.setscrcpyargs(false);




         if (startview)
             dialog.setstartview(true);
         else
             dialog.setstartview(false);


         dialog.setlinterm(dropdown.toInt());
         dialog.setmacterm(dropdown.toInt());
         dialog.setwinterm(dropdown.toInt());

         dialog.setdownloaddir(download);
         dialog.setlocaladb(localadb);
         dialog.setinstalldir(install);
         dialog.setbackupdir(backup);
          dialog.setdonation(donation);


         dialog.setversionLabel(version);

         dialog.setModal(true);

         if (dialog.exec() == QDialog::Accepted)
         {
             if (os == 1)
                obj["dropdown"] = dialog.winterm();
             if (os == 0)
                obj["dropdown"] = dialog.linterm();
             if (os == 2)
                obj["dropdown"] = dialog.macterm();

             obj["checkversion"] = dialog.versioncheck();

             obj["scrcpy"] = dialog.scrcpyargs();

             obj["startview"] = dialog.startview();


             obj["donation"] = dialog.donation();
             obj["download"] = dialog.downloaddir();
             obj["install"] = dialog.installdir();
             obj["backup"] = dialog.backupdir();
             obj["localadb"] = dialog.localadb();


             QJsonDocument doc(obj);

             QFile file(databasedir + "adblink.json");
             file.open(QIODevice::WriteOnly);
             file.write(doc.toJson());
             file.close();


              setDonateButtonActive(dialog.donation() != "jocala.com");

         }
    }



    ////////////////////////////////////////////////

    void MainWindow::on_actionDownload_Kodi_triggered()
    {




        QString link = "https://kodi.tv/download";

        QDesktopServices::openUrl(QUrl(link));



    }


    ////////////////////////////////////////////////////////

    void MainWindow::on_actionReiinstall_Busybox_triggered()
    {



     QString selectedDescription;
     if (!validateDeviceSelection(selectedDescription)) {
                              return;
     }

     DeviceRecord device = queryDeviceRecord(selectedDescription);




        QString busybox = '"' + QCoreApplication::applicationDirPath() + "/adbfiles/busybox" + '"';
        QString cstring;
        QString command;


        QMessageBox::StandardButton reply;
         reply = QMessageBox::question(this, "", "Re-install Busybox?",
                                       QMessageBox::Yes|QMessageBox::No);
         if (reply == QMessageBox::No)
         {

             return;
         }





        
        cstring = getadb() + " shell rm -r /data/local/tmp/adblink";
        command=getadbOutput(cstring);

        cstring = getadb() + " shell mkdir -p /data/local/tmp/adblink";
        command=getadbOutput(cstring);
        cstring = getadb() + " push "+busybox+ " /data/local/tmp/adblink/";





              command=getadbOutput(cstring);

              if (!command.contains("bytes"))
                {
                   logfile("busybox install failed ");
                   logfile(command);
                   
                   QMessageBox::critical(0,"","busybox install failed. See log.");
                   return;
                  }
             else
              {
                  logfile(command);
                  cstring = getadb() + " shell chmod 755 /data/local/tmp/adblink/busybox";
                  command=getadbOutput(cstring);

                  // logfile(cstring);
                  logfile(command);

                   cstring = getadb() + " shell /data/local/tmp/adblink/busybox --install -s /data/local/tmp/adblink";
                   command=getadbOutput(cstring);

                   // logfile(cstring);
                   logfile(command);
              }

          busybox_permissions();


           QMessageBox::information(this,"","Busybox re-installed.");

          return;


    }





    ///////////////////////////////////////////////
    void MainWindow::busybox_permissions()
    {

    QString cstring;
    QString command;

    cstring = getadb() + " shell chmod 755 /data/local/tmp/adblink/busybox";
    command=getadbOutput(cstring);

    }


///////////////////////////////////////////////////////
    void MainWindow::infoArchitecture()
    {





    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
                   return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    battery();



    QString cstring;



    cstring = getadb() + " shell getprop ro.product.cpu.abi";
    QString archi=getadbOutput(cstring);
    QString android = QString::number(getandroid());
    QString adevice=devicename();
    QString manufact=manufacturer();

    QString scoped;

    QStringList list;


    if (!isScoped())
                   scoped = "false";
    else scoped = "true";


     QString kbase = "/sdcard/Android/data/";

    list.append(archi);
    list.append(android);
    list.append(adevice);
    list.append(manufact);
    list.append(battery());
    list.append(scoped);

    deviceinfoDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    dialog.setWindowTitle(device.description);

    dialog.devinfo(list);

    dialog.setModal(true);

    if(dialog.exec() == QDialog::Accepted)
    {
                   return;

    }


    // adb shell dumpsys battery
    // present == false no batt
    //ro.product.product.device
    //ro.product.manufacturer
    // ro.product.brand
    //  adb shell getprop ro.build.version.release


    }



////////////////////////////////////////////

  void MainWindow::pushTimers_clicked()
    {

        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
         return;
        }



        DeviceRecord device = queryDeviceRecord(selectedDescription);

      // sleep
      //  getadbpath() shell settings put secure sleep_timeout 123456789
      // screensaver
      //  adb shell settings put system screen_off_timeout 123456789


        // getadbpath() shell settings get secure sleep_timeout
       //   adb shell settings get system screen_off_timeout


        // settings put global stay_on_while_plugged_in 1

       //  settings put global ambient_experience_enabled 0

       // android 11  put system screen_off_timeout  2147483647

           QString cstring;
           QString command;
           QString android = QString::number(getandroid());


           //  dialog.setdownloaddir(getdownloadpath());

       //    dialog.setversionLabel(version);

        //   qDebug() << android;

           sleepDialog dialog(this);
           dialog.setWindowModality(Qt::WindowModal);
           dialog.setFixedSize(450,300);


           if (android.toInt() < 11)
           {

           cstring = getadb() + " shell settings get secure sleep_timeout ";
           command=getadbOutput(cstring);
            command = command.simplified();
           command.replace( " ", "" );
           dialog.setcurrentsleep("Current: "+command);



           }


           else {

           cstring = getadb() + " shell settings get global stay_on_while_plugged_in ";
           command=getadbOutput(cstring);
           // qDebug() << command;
           command = command.simplified();
           command.replace( " ", "" );
           dialog.setcurrentsleep("Current: "+command);


           }




           cstring = getadb() + " shell settings get system screen_off_timeout ";
           command=getadbOutput(cstring);
           command = command.simplified();
           command.replace( " ", "" );
           dialog.setcurrentscreen("Current: "+command);

           dialog.setdevicelabel(device.description);
           dialog.setandroidlabel(android);




            if(dialog.exec() == QDialog::Accepted)
            {
                QString screenval = dialog.screenValue();
                QString sleepval = dialog.sleepValue();



                // settings put secure sleep_timeout 0
                // settings put system screen_off_timeout 2147460000



                if (android.toInt() < 11)
                  cstring = getadb() + " shell settings put secure sleep_timeout "+sleepval;
                else
                  cstring = getadb() + " shell settings put global stay_on_while_plugged_in "+sleepval;

                command=getadbOutput(cstring);

                cstring = getadb() + " shell settings put system screen_off_timeout "+screenval;
                command=getadbOutput(cstring);

                QString sleep1;

                if (android.toInt() < 11)
                {

                cstring = getadb() + " shell settings get secure sleep_timeout ";
                sleep1=getadbOutput(cstring);
                sleep1 = sleep1.simplified();
                sleep1.replace( " ", "" );

                }
                else {
                cstring = getadb() + " shell settings get global stay_on_while_plugged_in ";
                sleep1=getadbOutput(cstring);
                sleep1 = sleep1.simplified();
                sleep1.replace( " ", "" );

                }




                cstring = getadb() + " shell settings get system screen_off_timeout ";
                QString screen1=getadbOutput(cstring);
                screen1 = screen1.simplified();
                screen1.replace( " ", "" );


                QMessageBox::information(this,"","Sleep/Screensaver values adjusted");


            }




    }

//////////////////////////////////////////////////////

    void MainWindow::on_actiondelthumb_triggered()
    {
            QString selectedDescription;
            if (!validateDeviceSelection(selectedDescription)) {
                return;
            }

            DeviceRecord device = queryDeviceRecord(selectedDescription);

        QString cstring;
        QString command;
        QString mcpath;




        cstring = getadb() + " shell ls /sdcard/xbmc_env.properties";
        if(getreturncode(cstring))
        {  cstring = getadb() + " shell cat /sdcard/xbmc_env.properties";
            command=getadbOutput(cstring);
            command.replace(QRegExp("[\r\n]"), "");
            mcpath = command.mid(command.indexOf("xbmc.data=") + 10);
            mcpath=mcpath+"/.kodi";
        }
        else

        {
            mcpath="/sdcard/Android/data/" + device.xbmcpackage+"/files/.kodi";

        }





        QString thumb = mcpath+"/userdata/Thumbnails";
        QString textures = mcpath+"/userdata/Database/Textures*.db";

        cstring = getadb() +" shell ls "+thumb;
        command=getadbOutput(cstring);



       if (command.contains("No such file or directory"))
        {
              QMessageBox::critical(this,"","Thumbnails not found!");
              return;
       }







        QMessageBox::StandardButton reply2;
           reply2 = QMessageBox::question(this, "", "Delete Thumbnails?",
                                        QMessageBox::Yes|QMessageBox::No);
           if (reply2 == QMessageBox::Yes)
             {
               logfile("Removing Thumbnails");
               cstring = getadb() +" shell rm -r " + thumb;
               command=RunLongProcess(cstring,"Removing Thumbnails");
               if (command.length() > 0)
                  logfile("Thumbnail directory issue: "+command);
               cstring = getadb() +" shell rm -r " + textures;
               command=getadbOutput(cstring);
               if (command.length() > 0)
                  logfile("Textures database issue: "+command);
           }

           if (command.length() > 0)
              QMessageBox::critical(this,"","Errors. See log");
            else
              QMessageBox::information(this,"","Thumnails deleted");


    }




////////////////////////////////////////////////////////////////

    void MainWindow::on_actionCreate_kodi_data_triggered()

     {

        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
           return;
        }

        DeviceRecord device = queryDeviceRecord(selectedDescription);



        QString cstring;
        QString command;
        QString mcpath;
        QString kbase;

        mcpath ="/sdcard/kodi_data/" + device.xbmcpackage;
        kbase = "/sdcard/kodi_data/";

        cstring = cstring = getadb() +  " shell ps | grep "+device.xbmcpackage;

        command=getadbOutput(cstring);

        if (command.contains(device.xbmcpackage))
        {


           QMessageBox::StandardButton reply;
           reply = QMessageBox::question(this, "Stop Kodi", "Cannot create path while Kodi is running.\n Stop "+device.xbmcpackage+" on device?"  ,
                                         QMessageBox::Yes|QMessageBox::No);
           if (reply == QMessageBox::Yes)
           {


                    QString cstring = getadb() + " shell am force-stop "+device.xbmcpackage;
                    QString command=getadbOutput(cstring);
                    logfile(command);
           }


           else {


                    logfile(device.xbmcpackage+" running. Path creation failed");
                    return;
           }




        }


         cstring = getadb() + " shell ls "+mcpath;
         command=getadbOutput(cstring);

      if (!command.contains("No such file or directory"))
        {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Create Kodi Data", "This will overwrite /sdcard/kodi_data/\nProceed?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No)
           return;

        }

      if ( getandroid() >= 11 )
        {

        if ( is_package(device.xbmcpackage) )
        {
        cstring = getadb()+ " shell appops set --uid "+  device.xbmcpackage +" MANAGE_EXTERNAL_STORAGE allow";
        if (!getreturncode(cstring))
          {
             QMessageBox::critical(this, "", "Error setting Kodi permissions");
           // return;
          }
        }
      }


        cstring = getadb() + " shell rm -r "+mcpath;
        command=RunLongProcess(cstring,"Preparing target");
        logfile(command);

        cstring = getadb() + " shell ls "+mcpath;

        command=getadbOutput(cstring);


        if (command.contains("No such file or directory"))
        {
           cstring = getadb() + " shell mkdir -p "+mcpath+"/files/.kodi";
           command=getadbOutput(cstring);
           logfile(command);
           QString errorp = command;
           cstring = getadb() + " shell ls "+mcpath+"/files/.kodi";
           command=getadbOutput(cstring);



           if (command.contains("No such file or directory"))
           {
                    QMessageBox::critical(this,"","Error creating Kodi data folder");
                    logfile("Restore error:"+ errorp);
                    return;
           }

        } // nuke existing






        cstring = getadb() + " shell echo xbmc.data="+mcpath+"/files > /sdcard/xbmc_env.properties";
        command=getadbOutput(cstring);
        logfile("create /sdcard/xbmc_env.properties");
        logfile(command);


        QMessageBox::information(this,"","Kodi data area created");

    }











    ////////////////////////////////////////

       void MainWindow::doConsole_clicked()
       {


           QJsonObject obj;
           QJsonDocument doc(obj);
           QFile file(databasedir+"adblink.json");
           file.open(QIODevice::ReadOnly);
           doc = QJsonDocument::fromJson(file.readAll());
           obj = doc.object();
           QString dropdown = obj["dropdown"].toString();
           int mcheck=dropdown.toInt();



           logfile("detaching console process");

            QString cstring = "";
            QString command ="";



            if (os == 1)

                   {

                   QString commstr = scriptdir+"/cpath.bat";
                   QFile file(commstr);
                       if(!file.open(QFile::WriteOnly |
                                     QFile::Text))
                       {
                           logfile("error creating cpath.bat!");
                           QMessageBox::critical(this,"","Error creating bat file!");
                           return;
                       }



                       QTextStream out(&file);

                      out  <<  "echo off"  << endl;
                      if(getlocaladb() == "")
                          out  << "set PATH=%PATH%;"+adbfiles+";"<< endl;



                       file.flush();
                       file.close();


                       QProcess::startDetached("cmd.exe", QStringList() << "/c" << "start" << ""  << commstr);

    }






            QString pathdir = QCoreApplication::applicationDirPath() +"/adbfiles";

              if (os == 2 || os == 0)
                {



                  QString commstr = scriptdir+"cpath.sh";


                  QFile file(commstr);

                      if(!file.open(QFile::WriteOnly |
                                    QFile::Text))
                      {
                          logfile("error creating cpath!");
                          QMessageBox::critical(this,"","Error creating sh file!");
                          return;
                      }



                      QTextStream out(&file);
                       out  << "#!/bin/sh" << endl;


                     if(getlocaladb() == "")
                        out  << "export PATH="+pathdir+":$PATH" << endl;



                     out  << "/bin/sh" << endl;


                      file.flush();
                      file.close();


                      cstring = "chmod 0755 " + commstr ;
                      QString command=getadbOutput(cstring);



              }



              if (os == 2)
                   {


             switch (mcheck)
              {
              case 0:
                cstring = "open -a Terminal.app "+scriptdir+"cpath.sh";
                 break;
              case 1:
                 cstring = "open -a iTerm.app "+scriptdir+"cpath.sh";
                 break;
              default:
                cstring = "open -a Terminal.app "+scriptdir+"cpath.sh";
             }

         }

              if (os == 0)
                   {

                  switch (mcheck)
                  {
                  case 0:
                    cstring = "/usr/bin/gnome-terminal --working-directory="+apphome+ " -x "+scriptdir+"cpath.sh";
                     break;
                  case 1:
                     cstring = "/usr/bin/xfce4-terminal --working-directory="+apphome+ " -x "+scriptdir+"cpath.sh";
                     break;
                 case 2:
                     cstring = "/usr/bin/konsole --workdir="+apphome+ " -e "+scriptdir+"cpath.sh";
                    break;
                 default:
                    cstring = "/usr/bin/gnome-terminal --working-directory="+apphome+ " -x "+scriptdir+"cpath.sh";
                 }

            }


              QProcess::startDetached(cstring);

       }

   ////////////////////////////////////////////////


       void MainWindow::adbshellButton_clicked()
       {

              QString port;
              QString daddr;

              QString selectedDescription;
              if (!validateDeviceSelection(selectedDescription)) {
                 return;
              }



              DeviceRecord device = queryDeviceRecord(selectedDescription);




              if (device.isusb) {
                 port = "";
                 daddr = device.daddr;
              } else {
                 port = device.port.isEmpty() ? "5555" : device.port;
                 daddr = device.daddr + ":" + port; }




              QJsonObject obj;
              QJsonDocument doc(obj);
              QFile file(databasedir + "adblink.json");
              file.open(QIODevice::ReadOnly);
              doc = QJsonDocument::fromJson(file.readAll());
              obj = doc.object();
              QString dropdown = obj["dropdown"].toString();
              int mcheck = dropdown.toInt();






              logfile("detaching console process");
              logfile(device.daddr + ":" + device.port);

              QString cstring = "";

              if (os == 1)
              {
                 dos_shell();
              }
              else
              {
                 QString commstr = scriptdir + "console.sh";
                 QFile::remove(commstr);
                 QFile file(commstr);

                 if (!file.open(QFile::WriteOnly))
                 {
                    logfile("error creating console.sh!");
                    QMessageBox::critical(this, "", "Error creating command file!");
                    return;
                 }

                 QTextStream out(&file);



                  out << "#!/bin/sh" << endl;
                   cstring = getadbpath() + " -s " + daddr + " shell -t \"export PATH=\\$PATH:/data/local/tmp/adblink; sh -i\"";
                   out << cstring << endl;



                 file.flush();
                 file.close();

                 cstring = "chmod 0755 " + commstr;
                 QString command = getadbOutput(cstring);

                 QString shelldir = '"' + scriptdir + "console.sh" + '"';

                 if (os == 0)
                 {
                    switch (mcheck)
                    {
                    case 0:
                    cstring = "/usr/bin/gnome-terminal --working-directory=" + apphome + " -x " + shelldir;
                    break;
                    case 1:
                    cstring = "/usr/bin/xfce4-terminal --working-directory=" + apphome + " -x " + shelldir;
                    break;
                    case 2:
                    cstring = "/usr/bin/konsole --workdir=" + apphome + " -e " + shelldir;
                    break;
                    default:
                    cstring = "/usr/bin/gnome-terminal --working-directory=" + apphome + " -x " + shelldir;
                    }
                 }

                 if (os == 2)
                 {
                    switch (mcheck)
                    {
                    case 0:
                    cstring = "open -a Terminal.app " + shelldir;
                    break;
                    case 1:
                    cstring = "open -a iTerm.app " + shelldir;
                    break;
                    default:
                    cstring = "open -a Terminal.app " + shelldir;
                    }
                 }

                 QProcess::startDetached(cstring);
              }
       }


    //////////////////////////////////////////////////////////

       void MainWindow::scpyButton_clicked()
       {


           logfile("starting scrcpy function");

              QString port;
              QString daddr;
              QString cstring;
              QString command;
              QString argval;
              QString sernum = "";


              QString selectedDescription;
              if (!validateDeviceSelection(selectedDescription)) {
                 return;
              }



              DeviceRecord device = queryDeviceRecord(selectedDescription);





              if (device.isusb) {
                 port = "";
                 daddr = device.daddr;
              } else {
                 port = device.port.isEmpty() ? "5555" : device.port;
                 daddr = device.daddr + ":" + port;
              }





              QJsonObject obj;
              QJsonDocument doc(obj);
              QFile file(databasedir + "adblink.json");
              file.open(QIODevice::ReadOnly);
              doc = QJsonDocument::fromJson(file.readAll());
              obj = doc.object();

              bool scrcpy = doc.object()["scrcpy"].toBool();
              QString dropdown = obj["dropdown"].toString();
              int mcheck = dropdown.toInt();


              QString scrcpybat = scriptdir + "scrcpy.bat";
              QString scrcpytxt = scriptdir + "scrcpy.txt";

              QString line = "";

              if (QFileInfo(scrcpytxt).exists())
              {
                 QFile argfile(scrcpytxt);
                 if (!argfile.open(QIODevice::ReadOnly)) {
                    QMessageBox::information(0, "error", argfile.errorString());
                 }

                 QTextStream in(&argfile);

                 while (!in.atEnd()) {
                    line = in.readLine();
                 }

                 argfile.close();
              }



              logfile("detaching scrcpy console process");







              sernum = " -s " + daddr +  " ";

              if (scrcpy)
              {
                 scpDialog dialog;

                 dialog.setArgs(line);

                 if (dialog.exec() == QDialog::Accepted)
                 {
                    argval = dialog.scpArgs();
                 }
                 else
                    return;
              }

              if (os == 1)
              {
                 QString commstr2 = scriptdir + "/scrcpy.txt";
                 QFile file2(commstr2);

                 if (!file2.open(QFile::WriteOnly | QFile::Text))
                 {
                    logfile("error creating scrcpy.txt!");
                    QMessageBox::critical(this, "", "Error saving arguments!");
                    return;
                 }

                 QTextStream out2(&file2);

                 out2 << argval << endl;

                 file2.flush();
                 file2.close();

                 QString commstr = scriptdir + "/scrcpy.bat";
                 QFile file(commstr);

                 if (!file.open(QFile::WriteOnly | QFile::Text))
                 {
                    logfile("error creating scrcpy.bat!");
                    QMessageBox::critical(this, "", "Error creating bat file!");
                    return;
                 }

                 QTextStream out(&file);

                 out << "set PATH=%PATH%;" + adbfiles + ";" + scrcpydir + ";" << endl;
                 out << "scrcpy.exe -s " + daddr + " " + argval << endl;

                 file.flush();
                 file.close();

                 QProcess::startDetached("cmd.exe", QStringList() << "/c" << "start" << "" << commstr);
              }

              QString pathdir = QCoreApplication::applicationDirPath() + "/adbfiles";

              if (os == 2 || os == 0)
              {
                 apphome = QDir::fromNativeSeparators(apphome);

                 QString commstr2 = scriptdir + "/scrcpy.txt";

                 QFile file2(commstr2);

                 if (!file2.open(QFile::WriteOnly | QFile::Text))
                 {
                    logfile("error saving arguments!");
                    QMessageBox::critical(this, "", "Error saving arguments!");
                    return;
                 }

                 QTextStream out2(&file2);

                 out2 << argval << endl;

                 file2.flush();
                 file2.close();

                 QString commstr = scriptdir + "/scrcpy.sh";

                 QFile file(commstr);

                 if (!file.open(QFile::WriteOnly | QFile::Text))
                 {
                    logfile("error creating cpath!");
                    QMessageBox::critical(this, "", "Error creating script!");
                    return;
                 }





                 QTextStream out(&file);
                 out << "#!/bin/sh" << endl;
                 out << "scrcpy -s " + daddr + " " + argval << endl;

                 file.flush();
                 file.close();

                 cstring = "chmod 0755 " + commstr;
                 QString command = getadbOutput(cstring);
              }

              if (os == 2)
              {
                 switch (mcheck)
                 {
                 case 0:
                    cstring = "open -a Terminal.app " + scriptdir + "/scrcpy.sh";
                    break;
                 case 1:
                    cstring = "open -a iTerm.app " + scriptdir + "/scrcpy.sh";
                    break;
                 default:
                    cstring = "open -a Terminal.app " + scriptdir + "/scrcpy.sh";
                 }
              }

              if (os == 0)
              {
                 switch (mcheck)
                 {
                 case 0:
                    cstring = "gnome-terminal --working-directory=" + apphome + " -x " + scriptdir + "/scrcpy.sh";
                    break;
                 case 1:
                    cstring = "xfce4-terminal --working-directory=" + apphome + " -x " + scriptdir + "/scrcpy.sh";
                    break;
                 case 2:
                    cstring = "konsole --workdir=" + apphome + " -e " + scriptdir + "/scrcpy.sh";
                    break;
                 default:
                    cstring = "gnome-terminal --working-directory=" + apphome + " -x " + scriptdir + "/scrcpy.sh";
                 }
              }

              QProcess::startDetached(cstring);
       }



///////////////////////////////////////////////////////////

 QString MainWindow::readBackup(QString databasedir) {
  QJsonObject obj;
  QJsonDocument doc(obj);
  QFile file(databasedir + "adblink.json");
  file.open(QIODevice::ReadOnly);
  doc = QJsonDocument::fromJson(file.readAll());
  obj = doc.object();
  QString backup = obj["backup"].toString();
  file.close();
  return backup;
}


///////////////////////////////
 void MainWindow::dos_shell()
 {
  QString sernum = "";
  QString port = "";
  QString daddr = "";

  QString selectedDescription;
  if (!validateDeviceSelection(selectedDescription)) {
                 return;
  }




  DeviceRecord device = queryDeviceRecord(selectedDescription);

  if (device.isusb) {
                 port = "";
                 daddr = device.daddr;
  } else {
                 port = device.port.isEmpty() ? "5555" : device.port;
                 daddr = device.daddr + ":" + port;
  }

  QString programName = QCoreApplication::applicationName();

  QString commstr = scriptdir + "/shell.bat";
  QFile file(commstr);

  if (!file.open(QFile::WriteOnly | QFile::Text)) {
                 logfile("error creating shell.bat!");
                 QMessageBox::critical(this, "", "Error creating bat file!");
                 return;
  }

  QTextStream out(&file);

  out << "echo off" << endl;

  if (getlocaladb() == "")
     out << "set PATH=%PATH%;" + adbfiles + ";" << endl;

  out << "adb.exe -s " + daddr + " shell -t \"export PATH=\\$PATH:/data/local/tmp/adblink; export PS1=\\$HOSTNAME:\\$PWD\\$\\ ; sh -i\"" << endl;



  file.flush();
  file.close();

  QProcess::startDetached("cmd.exe", QStringList() << "/c" << "start" << "" << commstr);
 }

///////////////////////////////////////////////////
void MainWindow::writeBackup (QString dir) {
    QDir adir = QDir(dir);
     adir.cdUp();
     QString backup = adir.absolutePath();
     QFile file(databasedir + "/adblink.json");
     file.open(QIODevice::ReadOnly);
     QByteArray data = file.readAll();
     QJsonDocument doc = QJsonDocument::fromJson(data);
     QJsonObject obj = doc.object();
     obj["backup"] = backup;
     doc.setObject(obj);
     file.close();
     file.open(QIODevice::WriteOnly);
     file.write(doc.toJson());
     file.close();


}


///////////////////////////////////////////////////////
QString MainWindow::readInstall(QString databasedir) {
 QJsonObject obj;
 QJsonDocument doc(obj);
 QFile file(databasedir + "adblink.json");
 file.open(QIODevice::ReadOnly);
 doc = QJsonDocument::fromJson(file.readAll());
 obj = doc.object();
 QString install = obj["install"].toString();
 file.close();
 return install;
}


/////////////////////////////////////////////////////
void MainWindow::writeInstall (QString install) {

    QFile file(databasedir + "/adblink.json");
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();
    obj["install"] = install;
    doc.setObject(obj);
    file.close();
    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson());
    file.close();


}

/////////////////////////////////////////////

void MainWindow::backupButton_clicked()
{

    QString cstring;
    QString command;
    QString mcpath;
    QString kbase;
    QString n_data_root;
    QString editport;
    QString port;

    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
                                   return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    if (!is_package(device.xbmcpackage)) {
                                   QMessageBox::critical(this, "", device.xbmcpackage + " not installed");
                                   logfile(device.daddr + ": Error: " + device.xbmcpackage + " not installed"); // Log error
                                   return;
    }

    // Use databasedir (unchanged from original)
    QString backup = readBackup(databasedir);

    if (!device.isusb) {
                                   if (device.port.isEmpty())
                  port = "5555";
                                   editport = ":" + port;
    }

    const QString adbPrefix = getadb() + " ";



    logfile("Starting backup for " + device.daddr); // Log start

    cstring = adbPrefix + "shell /data/local/tmp/adblink/busybox find /storage -type d -maxdepth 1";
    QString s = getadbOutput(cstring);
    QStringList list = s.split('\n');

    for (int i = 0; i < list.size(); i++) {
                                   list[i].remove('\r');
                                   list[i].remove('\n');
                                   if (list[i] == "Android" ||
                                       list[i] == "Permission denied" ||
                                       list[i] == "/storage/emulated" ||
                                       list[i] == "/storage" ||
                                       list[i] == "/storage/self" ||
                                       list[i].isEmpty()) { // Replaced NULL with isEmpty()
                  list.removeAt(i);
                  i--;
                                   }
    }

    n_data_root = "/sdcard";
    list.insert(0, "/sdcard");

    if (list.count() > 1) {
                                   restDialog dialog(this);
                                   dialog.setWindowModality(Qt::WindowModal);
                                   dialog.setWindowTitle("Backup for " + device.daddr); // Added device.daddr
                                   dialog.setadb_restore(list);
                                   if (dialog.exec() == QDialog::Accepted) {
                  n_data_root = dialog.restore_data_root();
                                   } else {
                  return;
                                   }
    }

    if (!n_data_root.startsWith("/")) {
                                   n_data_root.prepend("/");
    }
    if (!n_data_root.endsWith("/")) {
                                   n_data_root.append("/");
    }

    if (isScoped()) { // Replaced device.scoped with isScoped()
                                   mcpath = n_data_root + "kodi_data/" + device.xbmcpackage;
                                   kbase = n_data_root + "kodi_data/";
    } else {
                                   mcpath = n_data_root + "Android/data/" + device.xbmcpackage;
                                   kbase = n_data_root + "Android/data/";
    }

    cstring = adbPrefix + "shell ls " + mcpath + "/files/.kodi";
    if (!getreturncode(cstring)) {
                                   QMessageBox::critical(this, "", "Kodi's files not found at " + mcpath);
                                   logfile(device.daddr + ": Error: Kodi's files not found at " + mcpath); // Log error with device.daddr
                                   return;
    }

    QDir backupDir(backup);
    QString dir = QFileDialog::getExistingDirectory(this, "Choose Backup Destination for " + device.daddr, // Added device.daddr
                                                    backupDir.absolutePath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
                                   QMessageBox::StandardButton reply;
                                   reply = QMessageBox::question(this, "Backup", "Backup to " + dir + " for " + device.daddr + "?", // Added device.daddr
                                                                 QMessageBox::Yes | QMessageBox::No);
                                   if (reply == QMessageBox::Yes) {
                  mcpath = mcpath + "/";
                  dir = dir + "/";
                  if (os == 1) {
                    dir.replace("/", "\\");
                  }

                  cstring = adbPrefix + "pull " + mcpath + "files/.kodi/. " + '"' + dir + '"';
                 // command = RunLongProcess(cstring, "backup running");
                  command = RunLongProcess(cstring, "backup running for " + device.daddr);
                  if (QDir(dir + "userdata").exists()) { // Preserved original validation
                    writeBackup(dir);
                    QMessageBox::information(this, "", "Backup complete for " + device.daddr); // Added device.daddr
                    logfile("Backup completed successfully for " + device.daddr); // Log success
                  } else {
                    QMessageBox::critical(this, "", "Backup failed for " + device.daddr + ". See log."); // Added device.daddr
                    logfile(device.daddr + ": Error: Backup failed: " + command); // Log error
                    return;
                  }
                                   }
    } else {
                                   logfile(device.daddr + ": Error: No backup destination selected"); // Log error
    }
}


/////////////////////////////////////////////

void MainWindow::restoreButton_clicked() {
    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
                                   return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    if (!is_package(device.xbmcpackage)) {
                                   QMessageBox::critical(this, "", device.xbmcpackage + " not installed");
                                   logfile(device.daddr + ": Error: " + device.xbmcpackage + " not installed"); // Log error
                                   return;
    }

    QString cstring;
    QString command;
    QString n_data_root;
    QString mcpath;
    QString xbmcpath;
    QString kbase;
    bool xbmc_env = false;
    const QString adbPrefix = getadb() + " ";



    logfile("Starting restore for " + device.daddr); // Log restore start

    // Check for xbmc_env.properties
    cstring = adbPrefix + "shell ls /sdcard/xbmc_env.properties";
    if (getreturncode(cstring)) {
                                   cstring = adbPrefix + "shell cat /sdcard/xbmc_env.properties";
                                   command = getadbOutput(cstring);
                                   command.replace(QRegExp("[\r\n]"), "");

                                   int startIndex = command.indexOf("=") + 1;
                                   int endIndex = command.indexOf(".kodi") + 5;
                                   xbmcpath = command.mid(startIndex, endIndex - startIndex);

                                   QMessageBox::StandardButton reply;
                                   reply = QMessageBox::question(this, "xbmc properties", "xbmc_env.properties file found on " + device.daddr + ".\nUse its values?",
                                                                 QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
                                   if (reply == QMessageBox::Yes) {
                  xbmc_env = true;
                  mcpath = xbmcpath;
                                   } else if (reply == QMessageBox::No) {
                  xbmc_env = false;
                                   } else if (reply == QMessageBox::Cancel) {
                  return;
                                   }
    }

    // Check if Kodi is running
    cstring = adbPrefix + "shell ps | grep " + device.xbmcpackage;
    command = getadbOutput(cstring);
    if (command.contains(device.xbmcpackage)) {
                                   QMessageBox::StandardButton reply;
                                   reply = QMessageBox::question(this, "Stop Kodi", "Cannot restore while Kodi is running on " + device.daddr + ".\n Stop " + device.xbmcpackage + "?",
                                                                 QMessageBox::Yes | QMessageBox::No);
                                   if (reply == QMessageBox::Yes) {
                  cstring = adbPrefix + "shell am force-stop " + device.xbmcpackage;
                  command = getadbOutput(cstring);
                  // Removed intermediate logging of force-stop command
                                   } else {
                  logfile(device.daddr + ": Error: " + device.xbmcpackage + " running. Restore failed"); // Log error
                  return;
                                   }
    }

    // Determine storage root and path if no xbmc_env.properties
    if (!xbmc_env) {
                                   cstring = adbPrefix + "shell /data/local/tmp/adblink/busybox find /storage -type d -maxdepth 1";
                                   QString storageOutput = getadbOutput(cstring);
                                   QStringList storageList = storageOutput.split('\n');

                                   for (int i = 0; i < storageList.size(); i++) {
                  storageList[i].remove('\r');
                  storageList[i].remove('\n');
                  if (storageList[i] == "Android" ||
                      storageList[i] == "Permission denied" ||
                      storageList[i] == "/storage/emulated" ||
                      storageList[i] == "/storage" ||
                      storageList[i] == "/storage/self" ||
                      storageList[i].isEmpty()) {
                    storageList.removeAt(i);
                    i--;
                  }
                                   }

                                   storageList.insert(0, "/sdcard");

                                   if (storageList.count() > 1) {
                  restDialog dialog(this);
                  dialog.setWindowModality(Qt::WindowModal);
                  dialog.setWindowTitle("Restore for " + device.daddr);
                  dialog.setadb_restore(storageList);
                  if (dialog.exec() == QDialog::Accepted) {
                    n_data_root = dialog.restore_data_root();
                  } else {
                    return;
                  }
                                   }

                                   if (n_data_root.isEmpty()) {
                  n_data_root = "/sdcard";
                                   }

                                   if (!n_data_root.startsWith("/")) {
                  n_data_root.prepend("/");
                                   }
                                   if (!n_data_root.endsWith("/")) {
                  n_data_root.append("/");
                                   }

                                   if (isScoped()) {
                  kbase = n_data_root + "kodi_data/";
                  mcpath = kbase + device.xbmcpackage;
                  // Create kodi_data area
                  cstring = adbPrefix + "shell mkdir -p " + kbase;
                  command = getadbOutput(cstring);
                  if (command.contains("No such file or directory")) {
                    QMessageBox::critical(this, "", "Failed to create kodi_data directory on " + device.daddr);
                    logfile(device.daddr + ": Error creating kodi_data: " + command); // Log error
                    return;
                  }
                                   } else {
                  mcpath = n_data_root + "Android/data/" + device.xbmcpackage;
                  kbase = n_data_root + "Android/data/";
                                   }
    }

    // Select backup directory
    QString backup = readBackup(databasedir);
    QDir backupDir(backup);
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Backup Folder for ") + device.daddr,
                                                    backupDir.absolutePath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty()) {
                                   return;
    }

    // Validate backup
    if (!QDir(dir + "/userdata").exists()) {
                                   QMessageBox::critical(this, "", "Invalid backup for " + device.daddr + ". No userdata folder.");
                                   logfile(device.daddr + ": Error: Invalid backup. No userdata folder."); // Log error
                                   return;
    }
    if (!QDir(dir + "/addons").exists()) {
                                   QMessageBox::critical(this, "", "Invalid backup for " + device.daddr + ". addons folder not found.");
                                   logfile(device.daddr + ": Error: Invalid backup. addons folder not found."); // Log error
                                   return;
    }

    // Confirm restore
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Restore", "Restore this backup to " + device.daddr + "? This will overwrite existing Kodi data.",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
                                   return;
    }

    cstring = adbPrefix + "shell rm -r " + mcpath;
    command = RunLongProcess(cstring, "preparing target for " + device.daddr);
    // Removed intermediate logging of rm command

    cstring = adbPrefix + "shell ls " + mcpath;
    command = getadbOutput(cstring);

    if (command.contains("No such file or directory")) {
                                   cstring = adbPrefix + "shell mkdir -p " + mcpath + "/files/.kodi";
                                   command = getadbOutput(cstring);
                                   // Removed intermediate logging of mkdir command
                                   QString errorOutput = command;
                                   cstring = adbPrefix + "shell ls " + mcpath + "/files/.kodi";
                                   command = getadbOutput(cstring);

                                   if (command.contains("No such file or directory")) {
                  QMessageBox::critical(this, "", "Error creating restore point on " + device.daddr);
                  logfile(device.daddr + ": Error creating restore point: " + errorOutput); // Log error
                  return;
                                   }
    }

    dir = dir + "/.";

    cstring = adbPrefix + "push \"" + dir + "\" " + mcpath + "/files/.kodi/";
    command = RunLongProcess(cstring, "restore running for " + device.daddr);
    // Removed intermediate logging of push command

    // Check restore success
    if (command.contains("bytes")) {
                                   cstring = adbPrefix + "shell rm /sdcard/xbmc_env.properties";
                                   command = getadbOutput(cstring);

                                   // Always create xbmc_env.properties for scoped devices
                                   if (isScoped()) {
                  cstring = adbPrefix + "shell echo xbmc.data=" + mcpath + "/files > /sdcard/xbmc_env.properties";
                  command = getadbOutput(cstring);
                  // Removed intermediate logging of xbmc_env.properties creation
                  if (command.contains("No such file or directory") || !command.isEmpty()) {
                    QMessageBox::critical(this, "", "Failed to create xbmc_env.properties on " + device.daddr);
                    logfile(device.daddr + ": Error creating xbmc_env.properties: " + command); // Log error
                    return;
                  }
                                   } else if (n_data_root != "/sdcard/") {
                  cstring = adbPrefix + "shell echo xbmc.data=" + mcpath + "/files > /sdcard/xbmc_env.properties";
                  command = getadbOutput(cstring);
                  // Removed intermediate logging of xbmc_env.properties creation
                                   }

                                   writeBackup(dir);
                                   QMessageBox::information(this, "", "Restore complete for " + device.daddr);
                                   logfile("Restore completed successfully for " + device.daddr); // Log successful completion
    } else {
                                   QMessageBox::critical(this, "", "Restore failed for " + device.daddr + ". See log.");
                                   logfile(device.daddr + ": Error: Restore failed: " + command); // Log error
    }
}


//////////////////////////////////////////////
void MainWindow::mvdataButton_clicked()
{

    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
           return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);





    QString cstring = getadb() + " shell ps | grep "+device.xbmcpackage;
    QString command=getadbOutput(cstring);

    QString destination;
    QString source;
    QString kbase;
    QString n_data_root;
    int choice;

    if (command.contains(device.xbmcpackage))
    {
           QMessageBox::StandardButton reply;
           reply = QMessageBox::question(this, "Stop Kodi", "Cannot move data while Kodi is running.\n Stop "+device.xbmcpackage+" on device?"  ,
                                         QMessageBox::Yes|QMessageBox::No);
           if (reply == QMessageBox::Yes)
           {


            QString cstring = getadb() + " shell am force-stop "+device.xbmcpackage;
            QString command=getadbOutput(cstring);
            logfile(command);
           }


           else {

            logfile(device.xbmcpackage+" running. Move data failed");
            return;
           }


    }



    cstring = getadb()+ " shell /data/local/tmp/adblink/busybox find /storage -type d -maxdepth 1";

    QString s = getadbOutput(cstring);



    QStringList list = s.split('\n');




    for (int i = 0; i < list.size(); i++) {

           list[i].remove('\r');
           list[i].remove('\n');

           if (list[i] == "Android" ||
               list[i] == "Permission denied" ||
               list[i] == "/storage/emulated" ||
               list[i] == "/storage" ||
               list[i] == "/storage/self" ||
               list[i] == NULL)          {
            list.removeAt(i);
            i--;
           }
    }


    if (list.size() <= 0)
    {
           QMessageBox::critical(this,"","No external devices found");
           return;
    }

    dataDialog dialog(this);

    dialog.setadb_data(list);
    dialog.setWindowModality(Qt::WindowModal); // Set window modality

  //  dialog.setModal(true);



    if(dialog.exec() == QDialog::Accepted)
    {

           n_data_root = dialog.externalLocation();
           choice = dialog.returnval2();


    }

    else return;


    if(!n_data_root.startsWith("/"))
           n_data_root.prepend("/");

    if(!n_data_root.endsWith("/"))
           n_data_root.append("/") ;






    if (choice ==1  )  // sdcard to external
    {
           if (isScoped())
           {
            kbase="/sdcard/kodi_data/";
            source=kbase + device.xbmcpackage;
            destination = n_data_root + "kodi_data/" + device.xbmcpackage;
           }
           else {
            source="/sdcard/Android/data/" + device.xbmcpackage;
            destination = n_data_root + "Android/data/" + device.xbmcpackage;
            kbase=source;
           }
    }


    if (choice == 2  ) // external to sdcard
    {
           if (isScoped())
           {  destination="/sdcard/kodi_data/" + device.xbmcpackage;
            source = n_data_root + "kodi_data/" + device.xbmcpackage;
            kbase=n_data_root+"kodi_data/";
           }
           else {
            destination="/sdcard/Android/data/" + device.xbmcpackage;
            source = n_data_root + "Android/data/" + device.xbmcpackage;
            kbase=source;
           }
    }




    cstring = getadb() +" shell ls "+source+"/files/.kodi";

    if (!getreturncode(cstring))
    {
           QMessageBox::critical(this,"","Kodi's files not found at "+source);
           logfile("Data move: files not found at "+source);
           return;
    }


    cstring = getadb() +" shell ls "+destination+"/files/.kodi";


    if (getreturncode(cstring))
    {

           QMessageBox::StandardButton reply;
           reply = QMessageBox::question(this, "", "Kodi data already exists. Overwrite?",
                                         QMessageBox::Yes|QMessageBox::No);
           if (reply  == QMessageBox::No)
           {
            return;
           }

           else {

            cstring=getadb() +" shell rm -r "+destination;
            logfile("Erasing: "+cstring);
            command=RunLongProcess(cstring,"Preparing target");
            logfile(command);


           }

    }


    cstring = getadb()+ " shell mkdir -p "+destination+"/files";
    command=getreturncode(cstring);



    cstring = getadb() +" shell cp -r "+source+"/files/.kodi " + destination +"/files";
    logfile("Kodi file move:"+cstring);
    command=RunLongProcess(cstring,"Copying data to "+destination);

    cstring = getadb() +" shell test -e "+destination+"/files/.kodi";

    if (!getreturncode(cstring)) {
           QMessageBox::critical(this, "", "File copy failed. See log.");
           return;
    }


    cstring = getadb() +" shell test -e /sdcard/xbmc_env.properties";
    if (getreturncode(cstring)) {
           getreturncode(getadb()+" shell rm /sdcard/xbmc_env.properties");
    }






    cstring = getadb() + " shell echo xbmc.data="+destination+"/files"+ " > /sdcard/xbmc_env.properties";

    if (!cstring.contains("/sdcard/Android/data/org.xbmc.kodi"))
    {
           if(!getreturncode(cstring))
            logfile("ERROR: "+command);


    }






    QMessageBox::StandardButton reply2;
    reply2 = QMessageBox::question(this, "", "Erase "+kbase+"?",
                                   QMessageBox::Yes|QMessageBox::No);
    if (reply2  == QMessageBox::No)
    {
           QMessageBox::information(this,"","Data copy complete");
           return;
    }



    //qDebug() << "deleting " << kbase;
    cstring = getadb() +" shell rm -r "+kbase;

    command=RunLongProcess(cstring,"Erasing "+kbase);
    logfile(
        "Erasing: " + cstring);


    QMessageBox::information(this,"","Data move complete");


}


//////////////////////////////////////////


void MainWindow::on_actionView_Changelog_triggered()
{
    QString link = "http://jocala.com/changelog.txt";
    QDesktopServices::openUrl(QUrl(link));

}




//////////////////////////////////////////


QString MainWindow::checkslash(QString qpath)
{

    if(!qpath.startsWith("/"))
     qpath.prepend("/");

    if(!qpath.endsWith("/"))
     qpath.append("/") ;

    return qpath;
}


void MainWindow::on_actionSplash_Screen_triggered()
{

    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
     return;
    }



    DeviceRecord device = queryDeviceRecord(selectedDescription);


    if(!is_busybox())
    {
     QMessageBox::critical(0,"","Busybox installation failed.");
     return;
    }

    busybox_permissions();



    if (!is_package(device.xbmcpackage))
    { QMessageBox::critical(
         this,
         "",
         device.xbmcpackage+" not installed");
     return;
    }


    QString cstring;
    QString command;
    QString mcpath="";


    cstring = getadb() + " shell ls /sdcard/xbmc_env.properties";
    if(getreturncode(cstring))
    {  cstring = getadb() + " shell cat /sdcard/xbmc_env.properties";
     command=getadbOutput(cstring);
     command.replace(QRegExp("[\r\n]"), "");
     mcpath = command.mid(command.indexOf("xbmc.data=") + 10);
     mcpath=mcpath+"/.kodi";
    }
    else

    {
     mcpath="/sdcard/Android/data/" + device.xbmcpackage+"/files/.kodi";

    }




    mcpath=mcpath+"/media";

    cstring = getadb() + " shell ls "+mcpath;
    command=getadbOutput(cstring);


    cstring = getadb() + " shell ls "+mcpath;
    command=getadbOutput(cstring);




    if (command.contains("No such file or directory"))
    {
     cstring = getadb() + " mkdir -p "+ mcpath;
     command=getadbOutput(cstring);
    }




    QString fileName = QFileDialog::getOpenFileName(this,"Choose splash screen file", QDir::homePath(), tr("Files (*.png *.jpg *.jpeg)"));



    if (!fileName.isEmpty() )
    {


     QMessageBox::StandardButton reply;
     reply = QMessageBox::question(this, "Push", fileName+" selected. Continue?",
                                   QMessageBox::Yes|QMessageBox::No);
     if (reply == QMessageBox::Yes) {


            cstring = getadb() + " push "+'"'+fileName+'"'+ " "+mcpath+"/splash.png";

            command=RunLongProcess(cstring,"Splash Screen");

            // logfile(cstring);
            logfile(command);


            if (command.contains("bytes"))


            {



              QMessageBox::information(
                  this,
                  "",
                  "Splash screen installed." );
            }
            else

            {

              QMessageBox::critical(
                  this,
                  "",
                  "Splash screen installation failed.");}


     }

    }



}


void MainWindow::on_View_Changelog_triggered()
{
            QString link = "http://jocala.com/changelog.txt";
            QDesktopServices::openUrl(QUrl(link));
}







void MainWindow::on_actionGet_UID_from_APK_file_triggered()
{


 QString command;
 QString cstring;
 QStringList mstringlist;



 QString filename = QFileDialog::getOpenFileName(
    this,
    "Open APK File",
    QDir::homePath(),
     "APK Files (*.apk);;All Files (*)"
     );


 if( !filename.isEmpty() )
 {

            cstring = aapt + " dump badging  " + '"'+ filename+'"';
            command=getadbOutput(cstring);



            mstringlist=command.split(QRegExp("[\t\n\r]"),QString::SkipEmptyParts);


            for (QStringList::iterator it = mstringlist.begin();
                 it != mstringlist.end(); ++it)
            {
              QString item=*it;
              if (item.contains("package"))
              {
              QRegExp rx("(\\')");
              QStringList query = item.split(rx);
              QString packagename = query.at(1);
              QMessageBox::information(this, "",packagename);
              }
            }

 }




}


void MainWindow::on_actionSend_text_triggered()
{

 QString selectedDescription;
 if (!validateDeviceSelection(selectedDescription)) {
            return;
 }

 DeviceRecord device = queryDeviceRecord(selectedDescription);


 QString command;
 QString cstring;

 bool ok;
 QString text = QInputDialog::getText(this, tr("Text to Device"),
                                      tr("Send text:"), QLineEdit::Normal,
                                      QString(), &ok);
 if (ok && !text.isEmpty()) {

            text.replace(" ", "%s");
            cstring = getadb() + " shell input text " + text;
            command=getadbOutput(cstring);
            logfile(cstring);
            logfile(command);
 }

 return;

}




//////////////////////////////////////////////////////////////
void MainWindow::on_actionOculus_VR_triggered()
{


 QString selectedDescription;
 if (!validateDeviceSelection(selectedDescription)) {
            return;
 }

 DeviceRecord device = queryDeviceRecord(selectedDescription);


 QString cstring;
 QString command;
 QString android = QString::number(getandroid());
 QString battery1;
 QString battery2;
 QString b1;
 QString b2;
 bool questexists;

 bool notQuest=false;


 int power;
 int proximity;
 int guardian;
 int cpu;
 int gpu;
 int refresh;
 int texture;
 int recording;
 int fovlevel;
 int dynamicfov;
 int ratecap;
 int chromatic;
 int exper;
 // int link;

 QString customCaptureWidth;
 QString customCaptureHeight;
 QString customTextureWidth;
 QString customTextureHeight;
 QString custombitrate;
 QString fovdown;
 QString fovup;
 QString fovout;
 QString fovin;




 oculusDialog dialog(this);
 dialog.setWindowModality(Qt::WindowModal);


 cstring = getadb() + " shell dumpsys CompanionService | grep Battery";
 QString temp=getadbOutput(cstring);

 if (temp.contains("Can't find service: CompanionService"))
 {



            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Quest", "Not a Quest device. Proceed?"  ,
                                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::No)
            {

              return;
            }


            else {

              notQuest=true;

            }



 }


// quest json



 if (QFileInfo::exists(databasedir+"/quest.json"))
            questexists = true;
 else
            questexists = false;

 if (!questexists)
 {



            QJsonObject obj;


            obj["power"] = 0;
            obj["proximity"] = 0;
            obj["guardian"] = 0;
            obj["cpu"] = 0;
            obj["gpu"] = 0;
            obj["refresh"] = 0;
            obj["exper"] = 0;

            obj["texture"] = 0;

            obj["recording"] = 0;
            obj["fovlevel"] = 0;
            obj["dynamicfov"] = 0;
            obj["ratecap"] = 0;
            obj["chromatic"] = 0;


            obj["customCaptureHeight"] = "";
            obj["customCaptureWidth"] = "";



            obj["customTextureHeight"] = "";
            obj["customTextureWidth"] = "";




            obj["custombitrate"] = "";

            obj["fovdown"] = "";
            obj["fovup"] = "";
            obj["fovout"] = "";
            obj["fovin"] = "";


            QJsonDocument doc(obj);

            QFile file(databasedir+"quest.json");
            file.open(QIODevice::WriteOnly);
            file.write(doc.toJson());
            file.close();




 }



            QJsonObject obj;
            QJsonDocument doc(obj);
            QFile file(databasedir+"quest.json");
            file.open(QIODevice::ReadOnly);
            doc = QJsonDocument::fromJson(file.readAll());
            obj = doc.object();


            power = obj["power"].toInt();
            proximity = obj["proximity"].toInt();
            guardian = obj["guardian"].toInt();
            cpu = obj["cpu"].toInt();
            gpu = obj["gpu"].toInt();
            exper = obj["exper"].toInt();

            refresh = obj["refresh"].toInt();

            texture = obj["texture"].toInt();

            recording = obj["recording"].toInt();
            fovlevel = obj["fovlevel"].toInt();
            dynamicfov = obj["dynamicfov"].toInt();
            ratecap = obj["ratecap"].toInt();
            chromatic = obj["chromatic"].toInt();

            customCaptureHeight = obj["customCaptureHeight"].toString();
            customCaptureWidth=obj["customCaptureWidth"].toString();

            customTextureHeight = obj["customTextureHeight"].toString();
            customTextureWidth=obj["customTextureWidth"].toString();

            custombitrate=obj["custombitrate"].toString();
            fovdown=obj["fovdown"].toString();
            fovup=obj["fovup"].toString();
            fovout=obj["fovout"].toString();
            fovin=obj["fovin"].toString();





            file.close();



// --crop 1600:900:2017:510 -m 1600 -b 25M -d





     dialog.powerSet(power);
     dialog.proximitySet(proximity);
     dialog.guardianSet(guardian);
     dialog.cpuSet(cpu);
     dialog.gpuSet(gpu);
     dialog.refreshSet(refresh);
     dialog.experSet(exper);

     dialog.textureSet(texture);


     dialog.recordingSet(recording);
     dialog.fovlevelSet(fovlevel);
     dialog.dynamicfovSet(dynamicfov);
     dialog.ratecapSet(ratecap);
     dialog.chromaticSet(chromatic);

     dialog.customCaptureHeightSet(customCaptureHeight);
     dialog.customCaptureWidthSet(customCaptureWidth);

     dialog.customTextureHeightSet(customTextureHeight);
     dialog.customTextureWidthSet(customTextureWidth);


     dialog.bitrateSet(custombitrate);

      dialog.fovupSet(fovup);
      dialog.fovdownSet(fovdown);
      dialog.fovinSet(fovin);
      dialog.fovoutSet(fovout);

 int colonPos = temp.indexOf(":");


 if (colonPos != -1) {
           b1 = temp.mid(colonPos + 2).trimmed();

 } else {
            b1="";
 }






 cstring = getadb() + " shell dumpsys OVRRemoteService | grep Battery";
 battery2=getadbOutput(cstring);

 static QRegularExpression pattern("Battery:\\s*(\\d+)%");

 // Create a regular expression match iterator
 QRegularExpressionMatchIterator iterator = pattern.globalMatch(battery2);

 // Store results in QStrings
 QString rightBattery, leftBattery;

 // Iterate over matches
 int matchCount = 0;
 while (iterator.hasNext() && matchCount < 2) {
            QRegularExpressionMatch match = iterator.next();
            QString batteryPercentage = match.captured(1);

            // Store in respective QStrings
            if (matchCount == 0) {
              rightBattery = batteryPercentage;
            } else {
              leftBattery = batteryPercentage;
            }

            matchCount++;
 }


 b1 = "Headset:"+b1+ "%  "+"Controllers: L "+leftBattery+"%"+" R "+rightBattery+"%";




 if (!notQuest)
 {
            dialog.titleSet("Quest Headset");
            dialog.setbattery1label("Batteries: "+b1);
 }

 else
 {
            dialog.titleSet("Unknown device");
            dialog.setbattery1label("Battery levels not found");
 }





 // qDebug() << b1;


    bool execute_true=false;

 if(dialog.exec() == QDialog::Accepted)
 {





    if (dialog.clickedButton==0) {

              bool validInt;




              // FOV

              if (dialog.fovinSelected().toInt(&validInt) || dialog.fovinSelected() == "0" )
              {
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.eyeFovInward  "+dialog.fovinSelected();
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              }



              if (dialog.fovoutSelected().toInt(&validInt) || dialog.fovoutSelected() == "0" )
              {
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.eyeFovOutward  "+dialog.fovoutSelected();
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              }


              if (dialog.fovupSelected().toInt(&validInt) || dialog.fovupSelected() == "0" )
              {
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.eyeFovUp  "+dialog.fovupSelected();
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              }



              if (dialog.fovdownSelected().toInt(&validInt) || dialog.fovdownSelected() == "0" )
              {
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.eyeFovDown  "+dialog.fovdownSelected();
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              }

         // end FOV





              if (dialog.bitrateSelected().toInt(&validInt) || dialog.bitrateSelected() == "0" )
              {
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.capture.bitrate "+dialog.bitrateSelected();
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              }


// custom resolutions

// texture

              if (dialog.customTextureHeightSelected().toInt(&validInt) && dialog.customTextureWidthSelected().toInt(&validInt))
              {
                execute_true=true;
                cstring = getadb() + " shell setprop debug.oculus.textureWidth "+dialog.customTextureWidthSelected();
                command=getadbOutput(cstring);
                logfile(cstring);
                logfile(command);
                cstring = getadb() + " shell setprop debug.oculus.textureHeight "+dialog.customTextureHeightSelected();
                command=getadbOutput(cstring);
                logfile(cstring);
                logfile(command);
              }




              else {



           switch(dialog.textureSelected()) {
              case 0:
                 break;
              case 1:
                 execute_true=true;
                 cstring = getadb() + " shell setprop debug.oculus.textureWidth 1536";
                 command=getadbOutput(cstring);
                 logfile(cstring);
                 logfile(command);
                 cstring = getadb() + " shell setprop debug.oculus.textureHeight 1690";
                 command=getadbOutput(cstring);
                 logfile(cstring);
                 logfile(command);
                 break;
              case 2:
                 execute_true=true;
                 cstring = getadb() + " shell setprop debug.oculus.textureWidth 1440";
                 command=getadbOutput(cstring);
                 logfile(cstring);
                 logfile(command);
                 cstring = getadb() + " shell setprop debug.oculus.textureHeight 1584";
                 command=getadbOutput(cstring);
                 logfile(cstring);
                 logfile(command);
                 break;
              default:
                 break;
              }
         }


 // end texture

 // capture


         if (dialog.customCaptureHeightSelected().toInt(&validInt) && dialog.customCaptureWidthSelected().toInt(&validInt))
         {
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.capture.width "+dialog.customCaptureWidthSelected();
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              cstring = getadb() + " shell setprop debug.oculus.capture.height "+dialog.customCaptureHeightSelected();
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
         }



         else {

         switch(dialog.recordingSelected()) {
         case 0:
              break;
         case 1:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.capture.width 852";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              cstring = getadb() + " shell setprop debug.oculus.capture.height 480";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;

         case 2:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.capture.width 1280";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              cstring = getadb() + " shell setprop debug.oculus.capture.height 720";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;

         case 3:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.capture.width 1920";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              cstring = getadb() + " shell setprop debug.oculus.capture.height 1280";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;


         case 4:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.capture.width 1024";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              cstring = getadb() + " shell setprop debug.oculus.capture.height 1024";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;

         default:
              break;


         }
      }

     // end capture

// end resolutions





              switch(dialog.proximitySelected()) {
              case 0:
              break;
              case 1:
              execute_true=true;
              cstring = getadb() + " shell am broadcast -a com.oculus.vrpowermanager.prox_close";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
              case 2:
              execute_true=true;
              cstring = getadb() + " shell am broadcast -a com.oculus.vrpowermanager.automation_disable";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
              default:
              break;
              }

              switch(dialog.guardianSelected()) {
              case 0:
              break;
              case 1:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.guardian_pause 0";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
              case 2:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.guardian_pause 1";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
              default:
              break;
              }


              switch(dialog.powerSelected()) {
              case 0:
              break;
              case 1:
              execute_true=true;
              cstring = getadb() + " shell svc power stayon false";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
              case 2:
              execute_true=true;
              cstring = getadb() + " shell svc power stayon true";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
              case 3:
              execute_true=true;
              cstring = getadb() + " shell svc power stayon usb";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
              case 4:
              execute_true=true;
              cstring = getadb() + " shell svc power stayon ac";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
              case 5:
              execute_true=true;
              cstring = getadb() + " shell svc power stayon wireless";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
              default:
              break;
              }




            switch(dialog.refreshSelected()) {
            case 0:
              break;
            case 1:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.refreshrate 72";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
            case 2:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.refreshrate 90";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
            case 3:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.refreshrate 120";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;

            default:
              break;
            }







            switch(dialog.cpuSelected()) {
            case 0:
              break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                execute_true=true;
                cstring = getadb() + " shell setprop debug.oculus.cpuLevel "+ QString::number(dialog.cpuSelected()-1);
                command=getadbOutput(cstring);
                logfile(cstring);
                logfile(command);
              break;
            default:
              break;
            }


            switch(dialog.gpuSelected()) {
            case 0:
              break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.gpuLevel "+ QString::number(dialog.gpuSelected()-1);
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
            default:
              break;
            }




            switch(dialog.chromaticSelected()) {
            case 0:
              break;
            case 1:
              execute_true=true;

              cstring = getadb() + " shell setprop debug.oculus.forceChroma 0";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
            case 2:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.forceChroma 1";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;

            default:
              break;
            }



            switch(dialog.ratecapSelected()) {
            case 0:
              break;
            case 1:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.fullRateCapture 1";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
            case 2:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.fullRateCapture 0";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;

            default:
              break;
            }

    //    qDebug() << ;


            switch(dialog.experSelected() ) {
            case 0:
              break;
            case 1:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.experimentalEnabled 1";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
            case 2:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.experimentalEnabled 0";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;

            default:
              break;
            }


            switch(dialog.dynamicfovSelected()) {
            case 0:
              break;
            case 1:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.foveation.dynamic 1";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
            case 2:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.foveation.dynamic 0";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;

            default:
              break;
            }



            switch(dialog.fovlevelSelected()) {
            case 0:
              break;
            case 1:
              execute_true=true;

              cstring = getadb() + " shell setprop debug.oculus.foveation.dynamic 0";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);

              cstring = getadb() + " shell setprop debug.oculus.foveation.level 0";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
            case 2:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.foveation.dynamic 0";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);

              cstring = getadb() + " shell setprop debug.oculus.foveation.level 1";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
            case 3:
              execute_true=true;

              cstring = getadb() + " shell setprop debug.oculus.foveation.dynamic 0";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);

              cstring = getadb() + " shell setprop debug.oculus.foveation.level 2";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
            case 4:
              execute_true=true;
              cstring = getadb() + " shell setprop debug.oculus.foveation.dynamic 0";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);

              cstring = getadb() + " shell setprop debug.oculus.foveation.level 3";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
            case 5:
              execute_true=true;

              cstring = getadb() + " shell setprop debug.oculus.foveation.dynamic 0";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);

              cstring = getadb() + " shell setprop debug.oculus.foveation.level 4";
              command=getadbOutput(cstring);
              logfile(cstring);
              logfile(command);
              break;
            default:
              break;
            }





            if (execute_true)
               QMessageBox::information(this,"","Values adjusted.\n\nThese changes are not persistent. Reboot headset to return values to default.");

    }


           if (dialog.clickedButton==0 || dialog.clickedButton==1)

           {
              texture=dialog.textureSelected();
              recording=dialog.recordingSelected();
              fovlevel=dialog.fovlevelSelected();
              dynamicfov=dialog.dynamicfovSelected();
              ratecap=dialog.ratecapSelected();
              chromatic=dialog.chromaticSelected();
              proximity=dialog.proximitySelected();
              guardian=dialog.guardianSelected();

              exper=dialog.experSelected();
              power=dialog.powerSelected();
              cpu=dialog.cpuSelected();
              gpu=dialog.gpuSelected();
              refresh=dialog.refreshSelected();

              customCaptureWidth=dialog.customCaptureWidthSelected();
              customCaptureHeight=dialog.customCaptureHeightSelected();

              customTextureWidth=dialog.customTextureWidthSelected();
              customTextureHeight=dialog.customTextureHeightSelected();

              custombitrate=dialog.bitrateSelected();
              fovdown=dialog.fovdownSelected();
              fovup=dialog.fovupSelected();
              fovout=dialog.fovoutSelected();
              fovin=dialog.fovinSelected();





              QJsonObject obj;


              obj["power"] = power;
              obj["proximity"] = proximity;
              obj["guardian"] = guardian;
              obj["cpu"] = cpu;
              obj["gpu"] = gpu;
              obj["exper"] = exper;


              obj["customCaptureWidth"] = customCaptureWidth;
              obj["customCaptureHeight"] = customCaptureHeight;


              obj["customTextureWidth"] = customTextureWidth;
              obj["customTextureHeight"] = customTextureHeight;

              obj["custombitrate"] = custombitrate;





              obj["fovdown"]=fovdown;
              obj["fovup"]=fovup;
              obj["fovout"]=fovout;
              obj["fovin"]=fovin;

              obj["refresh"] = refresh;
              obj["texture"] = texture;
              obj["recording"] = recording;
              obj["fovlevel"] = fovlevel;
              obj["dynamicfov"] = dynamicfov;
              obj["ratecap"] = ratecap;
              obj["chromatic"] = chromatic;


              QJsonDocument doc(obj);

              QFile file(databasedir+"quest.json");
              file.open(QIODevice::WriteOnly);
              file.write(doc.toJson());
              file.close();

           }



 }





}




////////////////////////////////////////////////


void MainWindow::on_actionEdit_XML_triggered()
{


 QString selectedDescription;
 if (!validateDeviceSelection(selectedDescription)) {
           return;
 }



 int selectedRow = ui->deviceTable->currentRow();
 if (selectedRow >= 0 && ui->deviceTable->item(selectedRow, 0)) {
           selectedDescription = ui->deviceTable->item(selectedRow, 0)->text();
 } else {
           QMessageBox::critical(this, "", "No device selected in table");
           return;
 }
 DeviceRecord device = queryDeviceRecord(selectedDescription);


 QString tempfile1;
 QString tempfile2;
 QString xpath = "";
 QString fileName;
 QString cstring;
 QString command;
 QString mcpath="";



 cstring = getadb() + " shell ls /sdcard/xbmc_env.properties";
 if(getreturncode(cstring))
 {  cstring = getadb() + " shell cat /sdcard/xbmc_env.properties";
           command=getadbOutput(cstring);
           command.replace(QRegExp("[\r\n]"), "");
           mcpath = command.mid(command.indexOf("xbmc.data=") + 10);
           mcpath=mcpath+"/.kodi";
 }
 else

 {
           if (isScoped())
              mcpath=device.data_root + "kodi_data/" + device.xbmcpackage+"/files/.kodi";
           else
              mcpath=device.data_root + "Android/data/" + device.xbmcpackage+"/files/.kodi";

 }




 xpath = mcpath+"/userdata/";

 qDebug() << xpath;

 cstring = getadb() + " shell "+busypath+"busybox find " +xpath+ " -maxdepth 1 -name *.xml ";

 command=getadbOutput(cstring);




 QStringList filelist=command.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);

 if (command.isEmpty() || command.contains("No such file or directory"))
 { QMessageBox::critical(this,"","No files found");

           // logfile(cstring);
           logfile(command);
           logfile("no files found!");
           return;
 }



 listfileDialog fdialog(this);
 fdialog.setWindowModality(Qt::WindowModal);
 fdialog.setFilelist(filelist);
 fdialog.setDialogTitle("XML Files");

 if(fdialog.exec() == QDialog::Accepted)
 {

           fileName = fdialog.return_fitem();


           if (fileName.isEmpty())
           {
              QMessageBox::critical(this,"","No file selected");
              logfile("no file selected");
              return;
           }



           QString filename(fileName.mid(fileName.lastIndexOf("/")+1,fileName.length()));

           cstring = getadb() + " pull "+'"'+fileName+'"'+" "+'"'+scriptdir+'"'+"/"+filename;
           command=getadbOutput(cstring);


           if (!command.contains("bytes"))
           {
              logfile("edit failed");
              logfile(command);
              QMessageBox::critical(
                  this,
                  "",
                  "Edit failed "+command);
              return;
           }



           QFile file1(scriptdir+filename);

           if(!file1.open(QIODevice::ReadOnly | QIODevice::Text))
              return;

           QString xmlfile = file1.readAll();
           editorDialog dialog;
           dialog.seteditor(xmlfile);
           dialog.setfilename(filename);

           tempfile2 = scriptdir+"/"+filename;
           tempfile1 = scriptdir+"/"+filename+".bak";


           dialog.setModal(true);

           if(dialog.exec() == QDialog::Accepted)
           {


              QMessageBox::StandardButton reply;
              reply = QMessageBox::question(this, "Save","Save "+ fileName+"?",
                                            QMessageBox::Yes|QMessageBox::No);
              if (reply == QMessageBox::No)
               return;


              xmlfile = dialog.xmlfile();


              //QMessageBox::information(this,"",xmlfile);



              QFile::copy(scriptdir+"/"+filename, scriptdir+filename+".bak");
              QFile caFile( scriptdir+"/"+filename);
              caFile.open(QIODevice::WriteOnly | QIODevice::Text);
              QTextStream outStream(&caFile);
              outStream << xmlfile;
              caFile.close();

              cstring = getadb() + " push "+'"'+tempfile1+'"'+ " "+xpath;
              command=getadbOutput(cstring);
              logfile(command);

              if (!command.contains("bytes"))
              {  QMessageBox::critical(this,"","Backup of "+filename+ "failed. Edit abandoned." );
               logfile("Backup of "+filename+ "failed");
               return;
              }



              cstring = getadb() + " push "+'"'+tempfile2+'"'+ " "+xpath;
              command=getadbOutput(cstring);
              logfile(command);

              if (!command.contains("bytes"))
              {  QMessageBox::critical(this,"","Problem replacing "+filename+ ". Edit abandoned." );
               logfile("Problem replacing "+filename+ ". Edit abandoned." );
               return;
              }


           }



           QFile file2 (tempfile1);
           file2.remove();

           QFile file3 (tempfile2);
           file3.remove();


 }


}

//////////////////////////////////////////////

void MainWindow::on_actionScreen_Capture_triggered()
{

}

//////////////////////////

void MainWindow::on_actionKeypad_triggered()
{
  keypadButton_clicked();
}

//////////////////////////////////

QString MainWindow::getadb()
{


 QString gadb = "";
 QString port;
 QString editport = "";
 QString selectedDescription;
 int selectedRow = ui->deviceTable->currentRow();


           if (selectedRow >= 0 && ui->deviceTable->item(selectedRow, 0)) {
              selectedDescription = ui->deviceTable->item(selectedRow, 0)->text();
           } else {
              QMessageBox::critical(this, "Error", "No device selected in table");
              return "error";
           }

            QString daddr = ui->deviceTable->item(selectedRow, 1)->text();

            DeviceRecord device = queryDeviceRecord(selectedDescription);


            if (device.isusb) {
              port = "";
              daddr = device.daddr;
            } else {
              port = device.port.isEmpty() ? "5555" : device.port;
              daddr = device.daddr + ":" + port;
            }



        gadb = getadbpath() + " -s " + daddr;

 return gadb;
}

///////////////////////////////////////////////////
DeviceRecord MainWindow::queryDeviceRecord(const QString& description) {
 DeviceRecord record;
 QString quotedDescription = "\"" + description + "\"";
 QSqlQuery query;

 QString sqlstatement = "SELECT Id, daddr, pulldir, xbmcpackage, data_root, buffermode, buffersize, "
                        "bufferfactor, description, filepath, port, isusb, disableroot, flag1, flag2, ostype "
                        "FROM device WHERE description=" + quotedDescription;
 query.exec(sqlstatement);
 while (query.next()) {
               record.id = query.value("Id").toInt();
               record.daddr = query.value("daddr").toString();
               record.pulldir = query.value("pulldir").toString();
               record.xbmcpackage = query.value("xbmcpackage").toString();
               record.data_root = query.value("data_root").toString();
               record.buffermode = query.value("buffermode").toInt();
               record.buffersize = query.value("buffersize").toString();
               record.bufferfactor = query.value("bufferfactor").toString();
               record.description = query.value("description").toString();
               record.filepath = query.value("filepath").toString();
               record.port = query.value("port").toString();
               record.isusb = query.value("isusb").toBool();
               record.disableroot = query.value("disableroot").toBool();
               record.scoped = query.value("flag1").toBool();
               record.wsa = query.value("flag2").toBool();
               record.ostype = query.value("ostype").toString();
 }
 if (query.lastError().isValid()) {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:" + QString::number(query.lastError().number()));
 }


 return record;
}

///////////////////////////////////////


bool MainWindow::validateDeviceSelection(QString& selectedDescription) {
 // Check if any device is connected in deviceTable
 bool hasConnectedDevice = false;
 for (int i = 0; i < ui->deviceTable->rowCount(); ++i) {
               if (ui->deviceTable->item(i, 2) &&
                   ui->deviceTable->item(i, 2)->text() == "Connected") {
              hasConnectedDevice = true;
              break;
               }
 }
 if (!hasConnectedDevice) {
               QMessageBox::critical(this, "", "No devices connected");
               return false;
 }

 // Get selected description from deviceTable
 int selectedRow = ui->deviceTable->currentRow();
 if (selectedRow >= 0 && ui->deviceTable->item(selectedRow, 0)) {
               selectedDescription = ui->deviceTable->item(selectedRow, 0)->text();
 } else {
               QMessageBox::critical(this, "", "No device selected in table");
               return false;
 }

 // Check if the selected device is connected
 if (ui->deviceTable->item(selectedRow, 2) &&
     ui->deviceTable->item(selectedRow, 2)->text() != "Connected") {
               QMessageBox::critical(this, "", "Selected device is not connected");
               return false;
 }

 return true;
}



void MainWindow::on_actionSwitch_View_triggered()
{

 if (ui->stackedWidget->currentIndex() == 0) {

               QMessageBox::StandardButton reply;
               reply = QMessageBox::question(this, "Kodi", "Switch to Android View?",
                                             QMessageBox::Yes | QMessageBox::No);
               if (reply == QMessageBox::No) {
              return;
               }

               ui->stackedWidget->setCurrentIndex(1);
               ui->menuKodi->menuAction()->setVisible(false);



               ui->infoArchitecture->setEnabled(false);
               ui->infoArchitecture->setVisible(false);

 }


 else {

               QMessageBox::StandardButton reply;
               reply = QMessageBox::question(this, "Kodi", "Switch to Kodi View?",
                                             QMessageBox::Yes | QMessageBox::No);
               if (reply == QMessageBox::No) {
              return;
               }

               ui->stackedWidget->setCurrentIndex(0);
               ui->menuKodi->menuAction()->setVisible(true);



               ui->infoArchitecture->setEnabled(true);
               ui->infoArchitecture->setVisible(true);
 }

}









void MainWindow::screenCap()
{


      QString selectedDescription;
      if (!validateDeviceSelection(selectedDescription)) {
               return;
      }

      QJsonObject obj;
      QJsonDocument doc(obj);
      QFile file(databasedir + "adblink.json");
      file.open(QIODevice::ReadOnly);
      doc = QJsonDocument::fromJson(file.readAll());
      obj = doc.object();
      QString pulldir = obj["download"].toString();
      file.close();



      DeviceRecord device = queryDeviceRecord(selectedDescription);
      QString port = device.port.isEmpty() ? "5555" : device.port;
      QString daddr = device.daddr + ":" + port;

      QDateTime dateTime = QDateTime::currentDateTime();
      QString dtstr = dateTime.toString("yyyyMMdd_HHmmss");
      dtstr = dtstr + ".png";

      QString cstring = getadbpath() + " -s " + daddr + " shell screencap -p " + "/data/local/tmp/"+dtstr;
      logfile(cstring);

      QString command = getadbOutput(cstring);
      if (!command.isEmpty()) {
               logfile(command);
               QMessageBox::critical(this, "", "Screenshot failed: " + command);
               return;
      }

      if (!device.pulldir.isEmpty())
               pulldir = device.pulldir;




      cstring = getadb() + " pull "+ "/data/local/tmp/"+dtstr + " " + pulldir;
      command = getadbOutput(cstring);
      logfile(cstring);
      logfile(command);

      // Check if the pulled file exists
      QString localFilePath = pulldir + "/" + dtstr;
      QFileInfo fileInfo(localFilePath);
      if (!fileInfo.exists()) {
               logfile("Error: Pulled file does not exist at " + localFilePath);
               QMessageBox::critical(this, "", "Failed to pull screenshot: File not found at " + localFilePath);
               return;
      }

      cstring = getadb() + " shell rm " + "/data/local/tmp/"+dtstr;
      command = getadbOutput(cstring);

      logfile(cstring);
      logfile(command);

      QMessageBox::information(this, "", "Screenshot " + dtstr + " copied to " + pulldir);


}


//////////////////////////////////////

bool MainWindow::renameColumn(const QString& oldColumnName, const QString& newColumnName)
{



      QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
      db.setDatabaseName(dbstring);

      QSqlQuery query(db);
      query.prepare("PRAGMA table_info(device)");

      if (!query.exec()) {
               qDebug() << "Failed to query table info for device table:" << query.lastError().text();
               return false;
      }

      bool oldColumnExists = false;
      bool newColumnExists = false;

      while (query.next()) {
               QString columnName = query.value("name").toString();
               if (columnName == oldColumnName) {
              oldColumnExists = true;
               }
               if (columnName == newColumnName) {
              newColumnExists = true;
               }
      }

      if (oldColumnExists && !newColumnExists) {
               QString sql = QString("ALTER TABLE device RENAME COLUMN %1 TO %2")
                                 .arg(oldColumnName, newColumnName);
               if (!query.exec(sql)) {
              qDebug() << "Failed to rename column:" << query.lastError().text();
              return false;
               }
               qDebug() << "Successfully renamed column" << oldColumnName << "to" << newColumnName << "in device table";
      } else if (!oldColumnExists) {
               qDebug() << "Column" << oldColumnName << "does not exist in device table";
      } else if (newColumnExists) {
               qDebug() << "Column" << newColumnName << "already exists in device table";
      }

      return true;
}


////////////////////////////////////////////////////////////

void MainWindow::on_actionSet_Kodi_permissions_triggered()
{


           QString selectedDescription;
           if (!validateDeviceSelection(selectedDescription)) {
                      return;
           }

           DeviceRecord device = queryDeviceRecord(selectedDescription);


           QString flag;

           QString cstring;





           setpDialog dialog(this);
           dialog.setWindowModality(Qt::WindowModal);

            if (ui->stackedWidget->currentIndex() == 0)
                   dialog.setpname(device.xbmcpackage);
            else
                   dialog.setpname("");



           if(dialog.exec() == QDialog::Accepted)
           {

                      if (dialog.getbutton())
                          flag="allow";
                      else
                          flag="deny";


                      cstring = getadb()+ " shell appops set --uid "+  dialog.getpname() +" MANAGE_EXTERNAL_STORAGE "+flag;




                      if (!getreturncode(cstring))
                          QMessageBox::critical(this, "", "Error setting app permissions");
                      else
                          QMessageBox::information(this, "", "app permissions set");



           }





}

////////////////////////////////////

void MainWindow::serverlabel()
{
           QString cstring = getadbpath() + " devices";
           getadbOutput(cstring);

}

/////////////////////////////////////////////////////////////////

bool MainWindow::validateIPAddress(const QString& ipAddress) {

           QString normalized = ipAddress.trimmed();


           QRegularExpression ipRegex(
               // IPv4 pattern
               "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
               "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"
               "|"

               "^[a-zA-Z0-9][a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9]?"
               "(\\.[a-zA-Z0-9][a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9]?)*"
               "\\.[a-zA-Z0-9]{1,}$",
               QRegularExpression::CaseInsensitiveOption
               );

           return ipRegex.match(normalized).hasMatch();
}


////////////////////////////////////////

void MainWindow::delRecordButton_clicked()
{

   QString descrip;
   QString daddr;

   int selectedRow = ui->deviceTable->currentRow();
   if (selectedRow >= 0 && ui->deviceTable->item(selectedRow, 0)) {
                      descrip = ui->deviceTable->item(selectedRow, 0)->text();
   } else {
                      QMessageBox::critical(this, "", "No device selected in table");
                      return;
   }

   if (!descrip.isEmpty())
   {
                      QMessageBox::StandardButton reply;
                      reply = QMessageBox::question(this, "", "Delete " + descrip + "?",
                                                    QMessageBox::Yes | QMessageBox::No);

                      if (reply == QMessageBox::No)
                      {
                          return;
                      }


                      deleteRecord(descrip);


                      selectedRow = ui->deviceTable->currentRow();
                      daddr = ui->deviceTable->item(selectedRow, 1)->text();



                      QString cstring = getadbpath() + " disconnect "+daddr;
                      QString command=getadbOutput(cstring);
                      logfile (command);
                      logfile("disconnect: "+daddr);

                      ui->deviceTable->removeRow(selectedRow);
                      logfile(descrip + " is deleted");

   }
}


///////////////////////////////////////////////////


void MainWindow::createTables()
{
   logfile("Attempting to create tables for: " + dbstring);

   QSqlDatabase db = QSqlDatabase::database();
   if (!db.isOpen()) {
                      logfile("Error: Database not open");
                      return;
   }

   logfile("Database opened successfully: " + db.databaseName());

   // Check if the device table already exists
   QSqlQuery checkQuery(db);
   bool tableExists = false;
   if (checkQuery.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='device';") && checkQuery.next()) {
                      tableExists = true;
                      logfile("Device table already exists");
   } else if (checkQuery.lastError().isValid()) {
                      logfile("Error checking for device table: " + checkQuery.lastError().text());
                      return;
   }

   if (!tableExists) {
                      // Create the device table
                      QString sqlstatement = "CREATE TABLE IF NOT EXISTS device ("
                                             "Id INTEGER PRIMARY KEY, "
                                             "daddr TEXT, "
                                             "description TEXT NOT NULL UNIQUE, "
                                             "pulldir TEXT, "
                                             "xbmcpackage TEXT, "
                                             "data_root TEXT, "
                                             "buffermode INTEGER, "
                                             "buffersize TEXT, "
                                             "bufferfactor TEXT, "
                                             "filepath TEXT, "
                                             "port TEXT, "
                                             "isusb INTEGER, "
                                             "ostype TEXT, "
                                             "logfilename TEXT, "
                                             "disableroot INTEGER, "
                                             "flag1 TEXT, "
                                             "flag2 TEXT, "
                                             "flag3 TEXT, "
                                             "flag4 TEXT, "
                                             "flag5 TEXT)";

                      QSqlQuery query(db);
                      if (!query.exec(sqlstatement)) {
                          logfile("SQL statement: " + sqlstatement);
                          logfile("SQLite error: " + query.lastError().text());
                          logfile("SQLite error code: " + QString::number(query.lastError().number()));
                          logfile("Database file path: " + db.databaseName());
                      } else {
                          logfile("Successfully created new device table");
                          // Verify table creation
                          QSqlQuery verifyQuery(db);
                          if (verifyQuery.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='device';") && verifyQuery.next()) {
               logfile("Device table creation confirmed");
                          } else {
               logfile("Error: Device table not found after creation attempt");
                          }
                      }
   }
}


//////////////////////////////////////////////

void MainWindow::deleteRecord(QString descrip)

{


   QString sqlstatement;

   QString quote = "\"";

   descrip = quote+descrip+quote;
   QSqlQuery query;


   sqlstatement= "DELETE FROM device WHERE description=" + descrip;
   query.exec(sqlstatement);



}


/////////////////////////////////////////////////////
void MainWindow::on_Erase_adbLink_database_triggered()
{
   QMessageBox::StandardButton reply;
   reply = QMessageBox::question(0, "", "Erase device database?\n\nWARNING: This action will delete all device records. Are you sure you want to proceed?",
                                 QMessageBox::Yes|QMessageBox::No);
   if (reply == QMessageBox::Yes)
   {
                      QSqlQuery pquery;
                      QString sqlstatement = "DROP TABLE device";
                      pquery.exec(sqlstatement);
                      if (!pquery.isActive())
                      {
                          logfile("SQLERROR: " + pquery.lastError().text());
                          return;
                      }

                      createTables();
                      loadDeviceTable();

   }
}


//////////////////////////////////////////

void MainWindow::stopapp_clicked()
{
   QString selectedDescription;
   if (!validateDeviceSelection(selectedDescription)) {
                      return;
   }

   DeviceRecord device = queryDeviceRecord(selectedDescription);

   QString jsonPath = QDir(databasedir).filePath("adblink.json");
   QString stopapp = "org.xbmc.kodi"; // Default
   QJsonObject jsonObj;

   // Read existing JSON to preserve all fields
   QFile file(jsonPath);
   if (file.exists()) {
                      if (!file.open(QIODevice::ReadOnly)) {
                          QMessageBox::critical(this, "Error", "Cannot read adblink.json.");
                          return;
                      }
                      QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
                      file.close();
                      if (doc.isObject()) {
                          jsonObj = doc.object();
                          if (jsonObj.contains("stopapp")) {
               stopapp = jsonObj["stopapp"].toString();
                          }
                      } else {
                          qWarning() << "Invalid JSON in adblink.json";
                      }
   }

   forcequitDialog dialog(false, stopapp, this);
   dialog.setWindowModality(Qt::WindowModal);
   if (dialog.exec() != QDialog::Accepted) {
                      return;
   }

   QString package = dialog.packagename();
   if (package.isEmpty()) {
                      QMessageBox::critical(this, "Error", "Invalid package name.");
                      return;
   }

   QString cstring = getadb() + " shell am force-stop " + package;
   QString command = getadbOutput(cstring);
   if (command.contains("error", Qt::CaseInsensitive)) {
                      qWarning() << "ADB command failed: " << command;
                      QMessageBox::warning(this, "Warning", "Failed to stop application.");
   }
   logfile(cstring);
   logfile(command);

   // Update JSON only if stopapp changed
   if (stopapp != package) {
                      jsonObj["stopapp"] = package;
                      if (!file.open(QIODevice::WriteOnly)) {
                          qWarning() << "Cannot write to adblink.json";
                          QMessageBox::critical(this, "Error", "Cannot save configuration.");
                          return;
                      }
                      file.write(QJsonDocument(jsonObj).toJson());
                      file.close();
   }
}

/////////////////////////////////////////

void MainWindow::startapp_clicked()
{
   QString selectedDescription;
   if (!validateDeviceSelection(selectedDescription)) {
                      return;
   }

   DeviceRecord device = queryDeviceRecord(selectedDescription);

   QString jsonPath = QDir(databasedir).filePath("adblink.json");
   QString startapp = "org.xbmc.kodi/org.xbmc.kodi.Splash"; // Default
   QJsonObject jsonObj;

   // Read existing JSON to preserve all fields
   QFile file(jsonPath);
   if (file.exists()) {
                      if (!file.open(QIODevice::ReadOnly)) {
                          QMessageBox::critical(this, "Error", "Cannot read adblink.json.");
                          return;
                      }
                      QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
                      file.close();
                      if (doc.isObject()) {
                          jsonObj = doc.object();
                          if (jsonObj.contains("startapp")) {
               startapp = jsonObj["startapp"].toString();
                          }
                      } else {
                          qWarning() << "Invalid JSON in adblink.json";
                      }
   }

   forcequitDialog dialog(true, startapp, this);
   dialog.setWindowModality(Qt::WindowModal);
   if (dialog.exec() != QDialog::Accepted) {
                      return;
   }

   QString package = dialog.packagename();
   if (package.isEmpty()) {
                      QMessageBox::critical(this, "Error", "Invalid package name.");
                      return;
   }

   QString cstring = getadb() + " shell am start -n " + package;
   QString command = getadbOutput(cstring);
   if (command.contains("error", Qt::CaseInsensitive)) {
                      qWarning() << "ADB command failed: " << command;
                      QMessageBox::warning(this, "Warning", "Failed to start application.");
   }
   logfile(cstring);
   logfile(command);

   // Update JSON only if startapp changed
   if (startapp != package) {
                      jsonObj["startapp"] = package;
                      if (!file.open(QIODevice::WriteOnly)) {
                          qWarning() << "Cannot write to adblink.json";
                          QMessageBox::critical(this, "Error", "Cannot save configuration.");
                          return;
                      }
                      file.write(QJsonDocument(jsonObj).toJson());
                      file.close();
   }
}


////////////////////////////////

QString MainWindow::battery()
{
    QString cstring = getadb() + " shell dumpsys battery";
    QString output = getadbOutput(cstring);
    QString batteryLevel = "Unknown";
    QString batteryStatus = "";
    bool batteryPresent = false;

    // Use split with QString::SkipEmptyParts for compatibility with Qt 5.12
    QStringList lines = output.split('\n', QString::SkipEmptyParts);
    for (const QString& line : lines) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.contains("present", Qt::CaseInsensitive)) {
            QStringList parts = trimmedLine.split(':');
            if (parts.size() > 1 && parts[1].trimmed() == "true") {
                batteryPresent = true;
            }
        }
        if (trimmedLine.contains("level", Qt::CaseInsensitive)) {
            QStringList parts = trimmedLine.split(':');
            if (parts.size() > 1) {
                batteryLevel = parts[1].trimmed();
            }
        }
        if (trimmedLine.contains("status", Qt::CaseInsensitive)) {
            QStringList parts = trimmedLine.split(':');
            if (parts.size() > 1) {
                QString statusValue = parts[1].trimmed();
                if (statusValue == "1") {
                    batteryStatus = " (unknown)";
                } else if (statusValue == "2") {
                    batteryStatus = " (charging)";
                } else if (statusValue == "3") {
                    batteryStatus = " (discharging)";
                } else if (statusValue == "4") {
                    batteryStatus = " (not charging)";
                } else if (statusValue == "5") {
                    batteryStatus = " (full)";
                } else {
                    batteryStatus = " (status: " + statusValue + ")";
                }
            }
        }
    }

    return batteryPresent ? (batteryLevel + batteryStatus) : "Unknown";
}

////////////////////////////////////

void MainWindow::displayOff()
{


   QString cstring = getadb() + " shell input keyevent 26 ";
   QString command = getadbOutput(cstring);

}




bool MainWindow::usbConnected(QString daddr)
{


 QString cstring = getadbpath() + " devices";
   QString command = getadbOutput(cstring);
   return command.contains(daddr);
}



void MainWindow::on_actionReload_devices_triggered()
{

   loadDeviceTable();

}



///////////////////////////////////////////////



void MainWindow::loadDeviceTable()
{
   QString sqlstatement;
   QSqlQuery query;

   // Store current connected device IDs
   QSet<QString> connectedDeviceIds;
   for (int row = 0; row < ui->deviceTable->rowCount(); ++row) {
        if (ui->deviceTable->item(row, 2) &&
            ui->deviceTable->item(row, 2)->text() == "Connected" &&
            ui->deviceTable->item(row, 0)) {
            connectedDeviceIds.insert(ui->deviceTable->item(row, 0)->data(Qt::UserRole).toString());
        }
   }

   // Clear and setup table
   ui->deviceTable->clearContents();
   ui->deviceTable->setRowCount(0);
   ui->deviceTable->setColumnCount(3);
   ui->deviceTable->setHorizontalHeaderLabels(QStringList() << "Device" << "IP" << "Status");
   ui->deviceTable->verticalHeader()->setVisible(false);
   ui->deviceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
   ui->deviceTable->setShowGrid(true);
   ui->deviceTable->setSortingEnabled(false);
   ui->deviceTable->setSelectionMode(QAbstractItemView::SingleSelection);
   ui->deviceTable->setSelectionBehavior(QAbstractItemView::SelectRows);

   // Ensure table scales with parent widget
   ui->deviceTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

   // Reset margins and padding to minimize extra space
   ui->deviceTable->setStyleSheet("QTableWidget { margin: 0px; padding: 0px; } QHeaderView::section { padding: 0px; }");

   // Execute query
   sqlstatement = "SELECT id, description, daddr, isusb FROM device";
   if (!query.exec(sqlstatement)) {
        qDebug() << "Query failed:" << query.lastError().text();
        return;
   }

   // Populate table
   int row = 0;
   while (query.next()) {
        ui->deviceTable->insertRow(row);
        QString deviceId = query.value(0).toString();
        QString description = query.value(1).toString();

        QTableWidgetItem* descItem = new QTableWidgetItem(description);
        descItem->setData(Qt::UserRole, deviceId);
        ui->deviceTable->setItem(row, 0, descItem);

        bool isUsb = query.value(3).toBool();
        QString ip = isUsb ? "USB" : (query.value(2).toString().isEmpty() ? "N/A" : query.value(2).toString());
        ui->deviceTable->setItem(row, 1, new IpTableWidgetItem(ip));

        QString status;
        if (isUsb) {
            status = usbConnected(query.value(2).toString()) ? "Connected" : "Disconnected";
        } else {
            status = connectedDeviceIds.contains(deviceId) ? "Connected" : "Disconnected";
        }
        ui->deviceTable->setItem(row, 2, new QTableWidgetItem(status));
        row++;
   }

   // Calculate usable viewport width
   int totalWidth = ui->deviceTable->viewport()->width() - 2 * ui->deviceTable->frameWidth() - 16; // Increased buffer
   if (ui->deviceTable->verticalScrollBar()->isVisible()) {
        totalWidth -= ui->deviceTable->verticalScrollBar()->width();
   }

   // Get content-based widths
   ui->deviceTable->resizeColumnsToContents();
   int col0Width = ui->deviceTable->columnWidth(0);
   int col1Width = ui->deviceTable->columnWidth(1);
   int col2Width = ui->deviceTable->columnWidth(2);

   // Define minimum widths for readability
   const int minWidth = 120; // Ensure readable content
   col0Width = qMax(col0Width, minWidth);
   col1Width = qMax(col1Width, minWidth);
   col2Width = qMax(col2Width, minWidth);

   // Scale widths to fill viewport exactly
   int currentTotal = col0Width + col1Width + col2Width;
   if (currentTotal != totalWidth && totalWidth > 0) {
        double totalContentWidth = col0Width + col1Width + col2Width;
        int newCol0Width = qRound(static_cast<double>(col0Width) * totalWidth / totalContentWidth);
        int newCol1Width = qRound(static_cast<double>(col1Width) * totalWidth / totalContentWidth);
        int newCol2Width = totalWidth - (newCol0Width + newCol1Width);

        // Ensure minimum widths
        newCol0Width = qMax(newCol0Width, minWidth);
        newCol1Width = qMax(newCol1Width, minWidth);
        newCol2Width = qMax(newCol2Width, minWidth);

        // Final adjustment to prevent overflow
        int newTotal = newCol0Width + newCol1Width + newCol2Width;
        if (newTotal > totalWidth && totalWidth > 0) {
            double adjustScale = static_cast<double>(totalWidth) / newTotal;
            newCol0Width = qRound(static_cast<double>(newCol0Width) * adjustScale);
            newCol1Width = qRound(static_cast<double>(newCol1Width) * adjustScale);
            newCol2Width = totalWidth - (newCol0Width + newCol1Width);
        }

        ui->deviceTable->setColumnWidth(0, newCol0Width);
        ui->deviceTable->setColumnWidth(1, newCol1Width);
        ui->deviceTable->setColumnWidth(2, newCol2Width);
   }

   // Explicitly disable horizontal scrollbar
   ui->deviceTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

   ui->deviceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
   ui->deviceTable->setSortingEnabled(true);
   ui->deviceTable->sortItems(0, Qt::AscendingOrder);
   ui->deviceTable->viewport()->update();
}



/////////////////////////////////////////////////

void MainWindow::on_infoArchitecture_triggered()
{
   infoArchitecture();
}



/////////////////////////////////////////////////////

void MainWindow::connections()
{
   // grid 1
   connect(ui->fmButton, &QPushButton::clicked, this, &MainWindow::fmButton_clicked);
   connect(ui->adbshellButton1, &QPushButton::clicked, this, &MainWindow::adbshellButton_clicked);
   connect(ui->backupButton, &QPushButton::clicked, this, &MainWindow::backupButton_clicked);
   connect(ui->restoreButton, &QPushButton::clicked, this, &MainWindow::restoreButton_clicked);
   connect(ui->sideload_Button, &QPushButton::clicked, this, &MainWindow::sideload_Button_clicked);
   connect(ui->uninstall_Button, &QPushButton::clicked, this, &MainWindow::uninstall_Button_clicked);
   connect(ui->mvdataButton, &QPushButton::clicked, this, &MainWindow::mvdataButton_clicked);
   connect(ui->pushTimers, &QPushButton::clicked, this, &MainWindow::pushTimers_clicked);
   connect(ui->screencap1, &QPushButton::clicked, this, &MainWindow::screenCap);
   connect(ui->stopADB, &QPushButton::clicked, this, &MainWindow::killServer_clicked);
   connect(ui->scpyButton, &QPushButton::clicked, this, &MainWindow::scpyButton_clicked);
   connect(ui->cacheButton, &QPushButton::clicked, this, &MainWindow::cacheButton_clicked);
   connect(ui->doConsole, &QPushButton::clicked, this, &MainWindow::doConsole_clicked);
   connect(ui->keypadButton, &QPushButton::clicked, this, &MainWindow::keypadButton_clicked);
   connect(ui->startapp, &QPushButton::clicked, this, &MainWindow::startapp_clicked);
   connect(ui->stopapp, &QPushButton::clicked, this, &MainWindow::stopapp_clicked);

   // grid2
   connect(ui->fmButton_2, &QPushButton::clicked, this, &MainWindow::fmButton_clicked);
   connect(ui->sideload_Button_2, &QPushButton::clicked, this, &MainWindow::sideload_Button_clicked);
   connect(ui->uninstall_Button_2, &QPushButton::clicked, this, &MainWindow::uninstall_Button_clicked);
   connect(ui->infoButton, &QPushButton::clicked, this, &MainWindow::infoArchitecture);

   connect(ui->screencap2, &QPushButton::clicked, this, &MainWindow::screenCap);
   connect(ui->stopADB2, &QPushButton::clicked, this, &MainWindow::killServer_clicked);
   connect(ui->startapp_2, &QPushButton::clicked, this, &MainWindow::startapp_clicked);
   connect(ui->stopapp_2, &QPushButton::clicked, this, &MainWindow::stopapp_clicked);
   connect(ui->adbshellButton_2, &QPushButton::clicked, this, &MainWindow::adbshellButton_clicked);
   connect(ui->doConsole_2, &QPushButton::clicked, this, &MainWindow::doConsole_clicked);
   connect(ui->textButton, &QPushButton::clicked, this, &MainWindow::on_actionSend_text_triggered);
   connect(ui->scpyButton_2, &QPushButton::clicked, this, &MainWindow::scpyButton_clicked);

   // adbconnect/edit
   connect(ui->connButton, &QPushButton::clicked, this, &MainWindow::connButton_clicked);
   connect(ui->disButton, &QPushButton::clicked, this, &MainWindow::disButton_clicked);
   connect(ui->newRecordButton, &QPushButton::clicked, this, [this]() {dataentry(true);});
   connect(ui->editRecordButton, &QPushButton::clicked, this, [this]() {dataentry(false);});
   connect(ui->delRecordButton, &QPushButton::clicked, this, &MainWindow::delRecordButton_clicked);
   connect(ui->clearAdhocButton, &QPushButton::clicked, this, &MainWindow::on_clearAdhocButton_clicked);

   // devices
   connect(ui->deviceTable, &QTableWidget::doubleClicked, this, &MainWindow::fmButton_clicked);

}



