
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
    #include "oslogdialog.h"
    #include "adbprefdialog.h"
    #include "sleepdialog.h"
    #include "oculusdialog.h"
    #include "scpdialog.h"
    #include "version.h"
    #include "program.h"
    #include "getadbdata.h"
    #include "logfile.h"
    #include "setpdialog.h"
    #include "adbutils.h"

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
//  #include <QStringRef>
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

    #ifdef Q_OS_LINUX
     int os=0;
    #elif defined(Q_OS_WIN)
      int os=1;
    #elif defined(Q_OS_MAC)
    int os=2;
    #endif


     QString vqurl = "http://www.jocala.com/version.txt";


    QRegularExpression validip( "[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}");


    bool isConnected = false;
    bool serverRunning = false;

    bool dbexists = false;
    bool jsonexists = false;
    bool updatecheck = true;
    bool versioncheck = true;
    bool isusb = false;
    bool disableroot = false;
    bool wsa = false;
    bool scoped = false;

    QString port = ""; 
    QString filename = "";
    QString apphome =  "";
    QString scriptdir = "";
    QString aapt = "";
    QString fastboot = "";
    QString xmldir = "";
    QString splashdir = "";
    QString tempdir="";


    QString hdir = "";
    QString daddr="";
    QString sldir = "";
    QString pushdir = "";
    QString pulldir = "";
    QString xbmcpackage ="";
    QString ostype = "";

    QString data_root = "";
    QString buffersize = "";
    QString bufferfactor = "";
    int buffermode = 1;

    QString dbstring = "";
    QString jsonstring = "";
    QString description = "";
    QString filepath = "";
    QString busypath="/data/local/tmp/adblink/";
    QString adblog;
    QStringList bufferlist;


    QString busybox;


    QString adbstr1 = "ADB server on  ";
    QString adbstr2 = "ADB server off ";

    QString adbfiles;
    QString logfiledir;
    QString databasedir;
    QString downloaddir;
    QString scrcpydir;


    int checkversion;





    QSqlDatabase db;

    struct adbRecieve {
        int returncode;
        QString returnText;
    };


    //////////////////////////////////////////////
    MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
    {

       connect(qApp, &QCoreApplication::aboutToQuit, this, &MainWindow::onApplicationQuit);

//ro.product.product.device
//ro.product.manufacturer
// ro.product.brand
//  adb shell getprop ro.build.version.release


        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

       adbfiles=QCoreApplication::applicationDirPath()+"/adbfiles/";
       aapt=QCoreApplication::applicationDirPath()+"/adbfiles/"+"aapt";
       aapt = '"'+aapt+'"';
       busybox = QCoreApplication::applicationDirPath()+"/adbfiles/busybox";
       busybox = '"'+busybox+'"';
       apphome = QCoreApplication::applicationDirPath();
       scrcpydir=QCoreApplication::applicationDirPath()+"/adbfiles/"+"scrcpy/";
       xmldir = adbfiles+"remotes/";
       splashdir = adbfiles+"splash/";
       tempdir = "/data/local/tmp/";

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






         ui->setupUi(this);
         setFixedSize(size());
         setWindowTitle(QCoreApplication::applicationName());



    #ifdef Q_OS_MAC
      //https://bugreports.qt.io/browse/QTBUG-51120
      ui->progressBar->setTextVisible(true);
    #endif //Q_OS_MAC



         ui->statusBar->addPermanentWidget(ui->server_running);
         ui->statusBar->addPermanentWidget(ui->progressBar);
         ui->progressBar->setHidden(true);
         ui->listRunningJobs->setHidden(true);


         QPixmap pix(":/assets/donatel.png");
         QIcon icon(pix);
         ui->donate->setIcon(icon);
         ui->donate->setText("");
         ui->donate->setIconSize(pix.size());



         ui->server_running->setText(adbstr2);

     //    qDebug() << program;

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


//=============================




      if (os == 1)
              {
              logfile("Windows");
              }

         if (os == 2)

              {
            logfile("macOS");
              }



         if (os == 0)

              {
            logfile("Linux");

              }






              logfile("------------");



      hdir = QDir::homePath();


      if (QFileInfo::exists(dbstring))
        dbexists = true;
      else
        dbexists = false;




      if (QFileInfo::exists(databasedir+"/adblink.json"))
          jsonexists = true;
      else
          jsonexists = false;


      QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
      db.setDatabaseName(dbstring);


      if (!db.open()) {
          QMessageBox::critical(0, qApp->tr("Cannot open database"),
         "Database error:\n"+dbstring, QMessageBox::Cancel);
         logfile("error opening database "+dbstring);
         return;
      }





      if (!dbexists)
       {
          createTables();
        }





      QSqlQuery query(db);

           if (db.tables().contains("preferences"))

           {


          QString databaseFile = databasedir + "/adblink.db";
          QString backupFile = databasedir + "/backup.db";

          QFileInfo fileInfo(databaseFile);
          if (fileInfo.exists())
            {
                 if (!QFile::exists(backupFile))
                 {
                    if (QFile::copy(databaseFile, backupFile))
                      logfile("Backup adblink database file created");
                   else
                      logfile("ERROR: Unable to create backup adblink database file");
                 }
             }




                             QJsonObject obj;


                             obj["checkversion"] = getadbPref();
                             obj["scrcpy"] = true;
                             obj["oldfm"] = false;



                             obj["dropdown"] = getmsgboxtype();

                             if (getdownloadpath() != NULL)
                              obj["download"] = getdownloadpath();
                             else  obj["download"] = QDir::homePath();



                             obj["install"] = QDir::homePath();;
                             obj["backup"] = QDir::homePath();

                            QJsonDocument doc(obj);

                            QFile file(databasedir+"adblink.json");
                            file.open(QIODevice::WriteOnly);
                            file.write(doc.toJson());
                            file.close();


                QString sqlstatement="DROP TABLE preferences";
                  query.exec(sqlstatement);
                   if(!query.isActive())
                      {   logfile("SQLERROR: "+query.lastError().text());
                           return; }

                   sqlstatement = "ALTER TABLE device ADD flag1 TEXT";
                      query.exec(sqlstatement);
                      if (!query.isActive()) {
                          logfile("SQLERROR: " + query.lastError().text());
                          return;
                      }




                      sqlstatement = "ALTER TABLE device ADD flag2 TEXT";
                      query.exec(sqlstatement);
                      if (!query.isActive()) {
                          logfile("SQLERROR: " + query.lastError().text());
                          return;
                      }

                      sqlstatement = "ALTER TABLE device ADD flag3 TEXT";
                      query.exec(sqlstatement);
                      if (!query.isActive()) {
                          logfile("SQLERROR: " + query.lastError().text());
                          return;
                      }

                      sqlstatement = "ALTER TABLE device ADD flag4 TEXT";
                      query.exec(sqlstatement);
                      if (!query.isActive()) {
                          logfile("SQLERROR: " + query.lastError().text());
                          return;
                      }

                      sqlstatement = "ALTER TABLE device ADD flag5 TEXT";
                      query.exec(sqlstatement);
                      if (!query.isActive()) {
                          logfile("SQLERROR: " + query.lastError().text());
                          return;
                      }


           }


           if (!jsonexists)
             {



                     QJsonObject obj;


                     obj["oldfm"] = false;
                     obj["checkversion"] = true;
                     obj["scrcpy"] = true;
                     obj["checkscope"] = true;
                     obj["startview"] = true;
                     obj["dropdown"] = "0";
                     obj["download"] = QDir::homePath();
                     obj["install"] = QDir::homePath();
                     obj["backup"] = QDir::homePath();


                     QJsonDocument doc(obj);

                    QFile file(databasedir+"adblink.json");
                    file.open(QIODevice::WriteOnly);
                    file.write(doc.toJson());
                    file.close();




           }


       connect(ui->deviceTable, &QTableWidget::doubleClicked, this, &MainWindow::on_fmButton_clicked);
       connect(ui->textButton, &QPushButton::clicked, this, &MainWindow::on_actionSend_text_triggered);
       connect(ui->infoButton, &QPushButton::clicked, this, &MainWindow::systeminfo);



     ui->newRecord->setShortcut(QKeySequence("Ctrl+B"));
     ui->editRecord->setShortcut(QKeySequence("Ctrl+C"));
     ui->delRecord->setShortcut(QKeySequence("Ctrl+D"));
     ui->connButton->setShortcut(QKeySequence("Ctrl+E"));
     ui->disButton->setShortcut(QKeySequence("Ctrl+F"));
     ui->fmButton->setShortcut(QKeySequence("Ctrl+G"));
    ui->adbshellButton->setShortcut(QKeySequence("Ctrl+I"));
    ui->sideload_Button->setShortcut(QKeySequence("Ctrl+L"));
    ui->uninstall_Button->setShortcut(QKeySequence("Ctrl+M"));
    ui->scpyButton->setShortcut(QKeySequence("Ctrl+R"));
    ui->doConsole->setShortcut(QKeySequence("Ctrl+T"));
    ui->stopapp->setShortcut(QKeySequence("Ctrl+U"));
    ui->startapp->setShortcut(QKeySequence("Ctrl+W"));
    ui->clearButton->setShortcut(QKeySequence("Ctrl+Y"));
    ui->stopADB->setShortcut(QKeySequence("Ctrl+Z"));


    connect(ui->fmButton_2, &QPushButton::clicked, this, &MainWindow::on_fmButton_clicked);
    connect(ui->adbshellButton_2, &QPushButton::clicked, this, &MainWindow::on_adbshellButton_clicked);
    connect(ui->sideload_Button_2, &QPushButton::clicked, this, &MainWindow::on_sideload_Button_clicked);
    connect(ui->uninstall_Button_2, &QPushButton::clicked, this, &MainWindow::on_uninstall_Button_clicked);
    connect(ui->stopADB2, &QPushButton::clicked, this, &MainWindow::on_killServer_clicked);
    connect(ui->scpyButton_2, &QPushButton::clicked, this, &MainWindow::on_scpyButton_clicked);
    connect(ui->doConsole_2, &QPushButton::clicked, this, &MainWindow::on_doConsole_clicked);
    connect(ui->startapp_2, &QPushButton::clicked, this, &MainWindow::on_startapp_clicked);
    connect(ui->stopapp_2, &QPushButton::clicked, this, &MainWindow::on_stopapp_clicked);
    connect(ui->stopADB, &QPushButton::clicked, this, &MainWindow::on_killServer_clicked);
    connect(ui->screencap1, &QPushButton::clicked, this, &MainWindow::on_actionScreen_Capture_triggered);
    connect(ui->screencap2, &QPushButton::clicked, this, &MainWindow::on_actionScreen_Capture_triggered);

    new QShortcut (QKeySequence("Ctrl+O"), this, SLOT(on_actionSend_text_triggered()));


    QFont buttonFont("Arial", 8); // Small size to fit 30x30
    ui->ascend->setFont(buttonFont);
    ui->descend->setFont(buttonFont);

    // Set button size and arrows
    ui->ascend->setFixedSize(30, 30);
    ui->descend->setFixedSize(30, 30);
    ui->ascend->setText(QString::fromUtf8("\u2193"));
    ui->descend->setText(QString::fromUtf8("\u2191"));

    // Style buttons to avoid clipping
    ui->ascend->setStyleSheet("padding: 2px; text-align: center;");
    ui->descend->setStyleSheet("padding: 2px; text-align: center;");




    if (program=="adblink")
    {
      ui->startapp->setShortcut(QKeySequence("Ctrl+V"));
      ui->mvdataButton->setShortcut(QKeySequence("Ctrl+N"));
      ui->backupButton->setShortcut(QKeySequence("Ctrl+J"));
      ui->restoreButton->setShortcut(QKeySequence("Ctrl+K"));

    }

    new QShortcut(QKeySequence("Ctrl+S"), this, SLOT(on_actionSend_text_triggered()));


        start_server();
        logfile("starting server");
        if (serverRunning)
         { ui->server_running->setText(adbstr1);
            }

        else
        { ui->server_running->setText(adbstr2);

        }


            ui->deviceTable->setColumnCount(3);
            ui->deviceTable->setHorizontalHeaderLabels(QStringList() << "Device" << "IP" << "Status");
            ui->deviceTable->verticalHeader()->setVisible(false);
            ui->deviceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
            ui->deviceTable->setShowGrid(true);




        loadDeviceTable();
        do_versioncheck();



     }


    MainWindow::~MainWindow()
    {



        logfile("closing program");
        kill_server();
        db.close();
         delete ui;


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




    /////////////////////////////////////////////////

    QString MainWindow::getDescription(QString mdaddr)
    {



        QSqlQuery query;
        QString sqlstatement;
        QString quote = "\"";
        QString descrip="";
        mdaddr = quote+mdaddr+quote;

        sqlstatement= "SELECT description FROM device WHERE daddr=" + mdaddr;
       // query.exec(sqlstatement);
      //  logfile(sqlstatement);

        query.prepare( sqlstatement );

        if (!query.exec()){
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
        }
        else
        {
            if( query.next( ) )
              descrip = query.value(0).toString();
        }


    return descrip;

    }




    /////////////////////////////////////////////////

    QString MainWindow::getDevice(QString mdaddr)
    {



        QSqlQuery query;
        QString sqlstatement;
        QString quote = "\"";
        QString fmdaddr("");
        mdaddr = quote+mdaddr+quote;

        sqlstatement= "SELECT daddr FROM device WHERE description=" + mdaddr;


        // query.exec(sqlstatement);
      //  logfile(sqlstatement);

        query.prepare( sqlstatement );

        if (!query.exec()){
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
        }
        else
        {
            if( query.next( ) )
              fmdaddr = query.value(0).toString();
        }


    return fmdaddr;

    }



    /////////////////////////////////////////////////

    QString MainWindow::getPort(QString mdaddr)
    {



    QSqlQuery query;
    QString sqlstatement;
    QString quote = "\"";
    QString fmport("");
    mdaddr = quote+mdaddr+quote;

    sqlstatement= "SELECT port FROM device WHERE description=" + mdaddr;


    query.prepare( sqlstatement );

    if (!query.exec()){
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
    }
    else
    {
            if( query.next( ) )
              fmport = query.value(0).toString();
    }


    return fmport;

    }

    ///////////////////////////////////////////


    bool MainWindow::getIsUsb(QString mdaddr)
    {
    QSqlQuery query;
    QString sqlstatement;
    QString quote = "\"";
    bool isusb = false; // Default value for boolean
    mdaddr = quote + mdaddr + quote;

    sqlstatement = "SELECT isusb FROM device WHERE description=" + mdaddr;

    query.prepare(sqlstatement);

    if (!query.exec()) {
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:" + QString::number(query.lastError().number()));
    } else {
            if (query.next()) {
              isusb = query.value(0).toInt() != 0; // Convert integer to boolean (0 = false, non-zero = true)
            }
    }

    return isusb;
    }

    /////////////////////////////////////////////////

    QString MainWindow::getOSType(QString mdaddr)

    {



        QSqlQuery query;
        QString sqlstatement;
        QString quote = "\"";
        QString fmostype;
        mdaddr = quote+mdaddr+quote;

        sqlstatement= "SELECT ostype FROM device WHERE description=" + mdaddr;

        query.prepare( sqlstatement );

        if (!query.exec()){
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
        }

            else
            {
                if( query.next( ) )
                  fmostype = query.value(0).toString();
            }


        int ost = QString(fmostype).toInt();

        if (ost > 3)
            fmostype="0";

        return fmostype;

    }


    /////////////////////////////////////////////////

    bool MainWindow::getflag1(QString mdaddr)

    {




        QSqlQuery query;
        QString sqlstatement;
        QString quote = "\"";
        QString fmscope("");
        mdaddr = quote+mdaddr+quote;
        bool iscope;
        sqlstatement= "SELECT flag1 FROM device WHERE description=" + mdaddr;

        query.prepare( sqlstatement );

        if (!query.exec()){
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
        }
        else
        {
            if( query.next( ) )
              fmscope = query.value(0).toString();
        }




        iscope = fmscope == "1";



    return iscope;

    }



    /////////////////////////////////////////////////

    QString MainWindow::getWSA(QString mdaddr)

    {



        QSqlQuery query;
        QString sqlstatement;
        QString quote = "\"";
        QString fmwsa("");
        mdaddr = quote+mdaddr+quote;

        sqlstatement= "SELECT flag2 FROM device WHERE description=" + mdaddr;

        query.prepare( sqlstatement );

        if (!query.exec()){
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
        }
        else
        {
            if( query.next( ) )
              fmwsa = query.value(0).toString();
        }


    return fmwsa;

    }







    /////////////////////////////////////////////////

    bool MainWindow::record_exists(QString descrip)
    {


        int record_is=0;
        QSqlQuery query;
        QString sqlstatement;
        QString quote = "\"";

        descrip = quote+descrip+quote;

    sqlstatement="SELECT EXISTS(SELECT 1 FROM device WHERE description=";
    sqlstatement=sqlstatement+descrip+" LIMIT 1)";

        query.prepare( sqlstatement );

        if (!query.exec()){
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
        }
        else
        {
            if( query.next( ) )
               record_is = query.value(0).toInt();
        }



    if(record_is==1)
    return true;
    else
    return false;

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



    //////////////////////////////////////////////
    bool MainWindow::isScoped()
    {
        bool scoped;
        scoped = getandroid() == 11 || getandroid() == 12;
        return scoped;
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




     ////////////////////////////////////////////

     void MainWindow::default_device_values()
     {
         ostype="0";
         scoped = "0";
         wsa="0";
         sldir = QDir::homePath();
         pushdir = QDir::homePath();
         port = "5555";
         filepath = "/files/.kodi";
         buffermode = 1;
         buffersize = "20971520";
         bufferfactor = "1";
         versioncheck=true;
         xbmcpackage = "org.xbmc.kodi";
         data_root="/sdcard/";
     }





     ////////////////////////////////////////////

     void MainWindow::blank_entry_form()
     {

         daddr="";
         ostype="0";
         description="";
         pulldir = "";
         isusb = false;
         filepath = "/files/.kodi";
         port = "";
         buffermode = 1;
         buffersize = "20971520";
         port = "5555";
         bufferfactor = "1";
         xbmcpackage = "org.xbmc.kodi";
         data_root="/sdcard/";
        disableroot = false;
        scoped=false;
        wsa=false;

     }


     //////////////////////////////////////////
     void MainWindow::kill_server()
    {

     QString cstring = getadbpath() + " kill-server";
     QString command=getadbOutput(cstring);
     logfile("server stopped");
     serverRunning = false;

    }


    //////////////////////////////////////
    void MainWindow::start_server()
    {



        QString cstring = getadbpath() + " kill-server";
        QString command=getadbOutput(cstring);

      //   logfile("server test");
      //   logfile(command);
      //   logfile(cstring);

           cstring = getadbpath() + " start-server";
          command=getadbOutput(cstring);

       //   logfile("server test");
      //    logfile(command);
       //   logfile(cstring);




         if (command.contains("daemon started successfully"))
            {
             //  logfile("server started");
             // logfile(cstring);
            //   logfile(command);
               serverRunning = true;
              }

             else
            {
             logfile("start-server failed!");
             logfile(cstring);
             logfile(command);
             serverRunning = false;
             }

       //  logfile("server test");
       //  logfile(command);
        // logfile(cstring);



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




    ////////////////////////////////
    int MainWindow::getRows()
    {

       int rowcount=0;
       QString sqlstatement = "SELECT Count(*) FROM device";

        QSqlQuery query;
        query.exec(sqlstatement);
        while (query.next()) {
              rowcount = query.value(0).toInt();
        }

        if (query.lastError().isValid())
         {
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
           }


        return rowcount;
    }




    ////////////////////////////////

    void MainWindow::createTables()
    {
     //   logfile("creating adblink.db");



        QString sqlstatement="CREATE TABLE device (Id INTEGER PRIMARY KEY, daddr text,description text NOT NULL UNIQUE,pulldir text,xbmcpackage text,data_root text,buffermode int, buffersize text, bufferfactor text, filepath text, port text,isusb int, ostype text, disableroot int, flag1 text,flag2 text,flag3 text,flag4 text,flag5 text)";


        QSqlQuery query;
        query.exec(sqlstatement);

        if (query.lastError().isValid())
         {
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
           }




    }


    ////////////////////////////////

    void MainWindow::insertDevice()
    {

      // logfile("Insert new record");

       QSqlQuery query;

        daddr.replace(QString("'"), QString("''"));
        description.replace(QString("'"), QString("''"));
        xbmcpackage.replace(QString("'"), QString("''"));
        pulldir.replace(QString("'"), QString("''"));
        xbmcpackage.replace(QString("'"), QString("''"));
        filepath.replace(QString("'"), QString("''"));
        port.replace(QString("'"), QString("''"));
        data_root.replace(QString("'"), QString("''"));
        ostype.replace(QString("'"), QString("''"));


        QString buffermd;
        QString usbflag;
        QString rootflag;
        QString wsaflag;
        QString scopeflag;

        buffermd.setNum(buffermode);
        usbflag.setNum(isusb);
        rootflag.setNum(disableroot);
        wsaflag.setNum(wsa);
        scopeflag.setNum(scoped);



    QString sql1  = "INSERT INTO device( daddr, description, pulldir, xbmcpackage, data_root, buffermode, buffersize, bufferfactor, filepath , port ,isusb,ostype,disableroot,flag1,flag2)";
    QString sql2 = " VALUES  ('"+daddr+"', '"+description+"', '"+pulldir+"', '"+xbmcpackage+"','"+data_root+"',  '"+buffermd+"',  '"+buffersize+"',  '"+bufferfactor+"', '"+filepath+"', '"+port+"','"+usbflag+"','"+ostype+"','"+rootflag+"','"+scopeflag+"','"+wsaflag+"' )";

    QString sqlstatement = sql1+sql2;

          query.exec(sqlstatement);

          if (query.lastError().isValid())
           {
             logfile(sqlstatement);
             logfile("SqLite error:" + query.lastError().text());
             logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
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
     //  logfile(sqlstatement);


    }


    /////////////////////////////////////

    void MainWindow::addcolumn1()
    {
    QString sqlstatement="SELECT rootpath FROM device ";
     QSqlQuery query;
     query.exec(sqlstatement);

     if (query.lastError().number() == 1)
      {
         sqlstatement = "ALTER TABLE device ADD COLUMN rootpath int";
         query.exec(sqlstatement);


         if (query.lastError().isValid())
          {
            logfile("rootpath column NOT added to device table");
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
            QMessageBox::critical(0,"","Can't alter databse. See log.");
         }
       else logfile("rootpath column added to device table");

     }

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






    //////////////////////////////////////////////////////////////////////
    void MainWindow::do_versioncheck() {


        QJsonObject obj;
        QJsonDocument doc(obj);
        QFile file(databasedir+"adblink.json");
        file.open(QIODevice::ReadOnly);
        doc = QJsonDocument::fromJson(file.readAll());
        obj = doc.object();
        bool checkversion = doc.object()["checkversion"].toBool();
        bool sortpref = doc.object()["sortpref"].toBool();
        bool startview = doc.object()["startview"].toBool();
        file.close();

        if (sortpref)
                  ui->csort->setChecked(true);
        else
                  ui->dsort->setChecked(true);





        if (startview) {
                  ui->stackedWidget->setCurrentIndex(1);
        } else {
                  ui->stackedWidget->setCurrentIndex(0);
        }




    if (checkversion)
       {
        QNetworkRequest request;
       request.setUrl(QUrl(vqurl));

       QNetworkAccessManager *nam = new QNetworkAccessManager();
       QNetworkReply *reply = nam->get(request);

       connect(reply, SIGNAL(finished()),
               this, SLOT(onReqCompleted()));
    }





    }




    /////////////////////////////////
    void MainWindow::onReqCompleted() {
       QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());


       if (reply->error() != QNetworkReply::NoError)
       {

           int err = reply->error();
           QString s2 = QString::number(err);
           QMessageBox::critical(0, "","Network error: " + s2,QMessageBox::Cancel);
           return;

       }

       QByteArray data = reply->readAll();

       QString s1(data);

           s1 = strip(s1);




           if (version != s1)
           {
               QDialog dialog;
               QVBoxLayout layout(&dialog); // Use QVBoxLayout for vertical arrangement
               QLabel messageLabel("adbLink version " + s1 + " is ready. Download?");
               layout.addWidget(&messageLabel);

               QHBoxLayout buttonLayout; // QHBoxLayout for buttons
               QPushButton yesButton("Yes");
               QPushButton noButton("No");
               QPushButton changelogButton("Changelog");

               buttonLayout.addWidget(&yesButton);
               buttonLayout.addWidget(&noButton);
               buttonLayout.addWidget(&changelogButton);

               layout.addLayout(&buttonLayout); // Add the button layout to the main layout

               QObject::connect(&yesButton, &QPushButton::clicked, [&]() {
                   QString link = "http://www.jocala.com";
                   QDesktopServices::openUrl(QUrl(link));
                   dialog.close();
               });

               QObject::connect(&noButton, &QPushButton::clicked, [&]() {
                   // Handle "No" button click
                   dialog.close();
               });

               QObject::connect(&changelogButton, &QPushButton::clicked, [&]() {
                   on_actionView_Changelog_triggered();
                   // dialog.close();
               });

               dialog.exec();
           }



         //  delete reply;

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


        if (device.ostype != "0") {
               QMessageBox::critical(this, "", "Android devices only.");
               return "";
        }




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
    void MainWindow::on_sideload_Button_clicked()
    {





        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
            return;
        }



        DeviceRecord device = queryDeviceRecord(selectedDescription);


        if (device.ostype != "0") {
            QMessageBox::critical(this, "Unavailable", "Android devices only.");
            return;
        }




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
    void MainWindow::on_uninstall_Button_clicked()
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



        if (device.ostype != "0") {
              QMessageBox::critical(this, "", "Android devices only.");
              return;
        }






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

                               QMessageBox::critical(this,"","Uninstall failed");
                            else
                              QMessageBox::information(this,"","Uninstalled");



                      }
    }


//////////////////////////////////////////////

    void MainWindow::on_connButton_clicked()
    {



                      QString cstring;
                      QString command;
                      QString s;
                      QString selectedDescription;
                      int selectedRow;
                      QString daddr;
                      QString port;


                     // Improved JSON file handling with error checking
                      QJsonObject obj;
                      QFile file(databasedir + "adblink.json");
                      bool checkscope = false; // Default value
                      if (!file.open(QIODevice::ReadOnly)) {
                            logfile("Failed to open adblink.json");
                            QMessageBox::critical(this, "", "Failed to read configuration file");
                            return;
                      }
                      QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
                      if (doc.isNull() || !doc.isObject()) {
                            logfile("Invalid JSON in adblink.json");
                            QMessageBox::critical(this, "", "Invalid configuration file format");
                            return;
                      }
                      obj = doc.object();
                      if (obj.contains("checkscope")) {
                            checkscope = obj["checkscope"].toBool();
                      } else {
                            logfile("checkscope key missing in adblink.json, using default false");
                      }
                      file.close();




                      // Handle ad-hoc IP input with validation
                      if (!ui->adhocip->text().isEmpty())
                      {
                            QString adhocIPText = ui->adhocip->text();
                            int colonIndex = adhocIPText.indexOf(':');
                            QString daddr, port;

                            if (colonIndex != -1) {
                              daddr = adhocIPText.left(colonIndex);
                              port = adhocIPText.mid(colonIndex + 1);
                            } else {
                              daddr = adhocIPText;
                              port = "5555";
                            }

                            // Validate IP address using isNull (avoids isValid error)
                            QHostAddress address(daddr);
                            if (daddr.isEmpty() || address.isNull()) {
                              logfile("Invalid IP address: " + daddr);
                              QMessageBox::critical(this, "", "Invalid IP address: " + daddr);
                              return;
                            }

                            // Validate port
                            bool ok;
                            int portNum = port.toInt(&ok);
                            if (!ok || portNum < 1 || portNum > 65535) {
                              logfile("Invalid port: " + port);
                              QMessageBox::critical(this, "", "Invalid port: " + port);
                              return;
                            }

                            cstring = getadbpath() + " connect " + daddr + ":" + port;
                            logfile(cstring);
                            command = connectadb(cstring);



                            if (command.contains("connected to"))
                            {
                              isConnected = true;
                              // Add new row to deviceTable with daddr and Connected
                              int newRow = ui->deviceTable->rowCount();
                              ui->deviceTable->insertRow(newRow);
                              ui->deviceTable->setItem(newRow, 0, new QTableWidgetItem("Ad hoc IP"));
                              ui->deviceTable->setItem(newRow, 1, new QTableWidgetItem(daddr));
                              ui->deviceTable->setItem(newRow, 2, new QTableWidgetItem("Connected"));
                              ui->deviceTable->clearSelection(); // Clear previous selections
                              ui->deviceTable->setCurrentCell(newRow, 0); // Set active cell
                              ui->deviceTable->selectRow(newRow); // Highlight the row
                              ui->deviceTable->setFocus(); // Ensure table has focus
                              logfile("Connected to " + daddr);
                              logfile("Android version: " + s.setNum(getandroid()));
                            }
                            else {


                              if (command.contains("failed to authenticate") || command.contains("ffline")) {
                                   logfile(command);
                                   cstring = getadbpath() + " disconnect " + daddr;
                                   command = connectadb(cstring);
                              }


                              isConnected = false;
                              logfile("Unable to connect to: " + daddr + ":" + port);
                              QMessageBox::critical(this, "", "Unable to connect to: " + daddr + ":" + port+"\nSee log.");
                            }

                            return;
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



                      if (device.isusb) {
                            port = "";
                            daddr = device.daddr;
                      } else {
                            port = device.port.isEmpty() ? "5555" : device.port;
                            daddr = device.daddr + ":" + port;
                      }






                      cstring = getadbpath() + " connect " + daddr;
                      command = connectadb(cstring);


                      if (command.contains("failed to authenticate") || command.contains("ffline")) {
                            isConnected = false;
                            ui->deviceTable->setItem(selectedRow, 2, new QTableWidgetItem(
                                                                         command.contains("failed to authenticate") ? "Unauthorized" : "Offline"));
                            QString cstring = getadbpath() + " disconnect " + daddr;
                            command = connectadb(cstring);
                            return;
                      }


                      logfile(cstring);
                      logfile(command);


                      if (command.contains("connected to")) {
                            isConnected = true;
                            ui->deviceTable->setItem(selectedRow, 2, new QTableWidgetItem("Connected"));


                            ui->deviceTable->clearSelection();
                            ui->deviceTable->setCurrentCell(selectedRow, 0);
                            ui->deviceTable->selectRow(selectedRow);
                            ui->deviceTable->setFocus();



                            logfile("Connected to " + daddr);
                            logfile("Android version: " + s.setNum(getandroid()));

                      } else {
                            isConnected = false;
                            ui->deviceTable->setItem(selectedRow, 2, new QTableWidgetItem("NA")); // Set Status to NA
                            logfile("Unable to connect to: " + daddr);
                            QMessageBox::critical(this, "", "Unable to connect to: " + daddr);
                      }

                      if (isConnected) {
                            ui->server_running->setText(adbstr1);
                            serverRunning = true;

                      }

                      // Scoped storage check with improved logging
                      if (checkscope && program == "adblink" && isScoped()) {

                            if (ui->adhocip->text().isEmpty())
                            {
                            cstring = getadb() + " shell ls /sdcard/xbmc_env.properties";
                            }
                              else {
                            cstring = getadbpath() + " -s " + ui->adhocip->text() + " shell ls /sdcard/xbmc_env.properties";

                              }



                            bool fileExists = getreturncode(cstring);
                            logfile("Checked xbmc_env.properties: " + QString(fileExists ? "Found" : "Not found"));
                            if (!fileExists) {
                              QMessageBox::StandardButton reply;
                              reply = QMessageBox::question(this, "Restore", "Scoped storage in effect\nCreate /sdcard/kodi_data?",
                                                            QMessageBox::Yes | QMessageBox::No);
                              if (reply == QMessageBox::Yes) {
                                 on_actionCreate_kodi_data_triggered();
                              }
                            }



                      }

    }


    ////////////////////////////////////////////////////////////////


    void MainWindow::on_disButton_clicked()
    {



   QMessageBox::StandardButton reply;
   reply = QMessageBox::question(this, "Disconnect", "Disconnect device?",
       QMessageBox::Yes | QMessageBox::No);
   if (reply == QMessageBox::No) {
           return;
     }



     if (!ui->adhocip->text().isEmpty())
     {
           daddr = ui->adhocip->text();
           QString cstring = getadbpath() + " disconnect " + daddr;
           QString command = getadbOutput(cstring);
           logfile("disconnect: " + daddr);
           logfile(command);

           int selectedRow = ui->deviceTable->currentRow();
           if (selectedRow >= 0 && ui->deviceTable->item(selectedRow, 0) &&
               ui->deviceTable->item(selectedRow, 0)->text() == daddr) {
                              ui->deviceTable->removeRow(selectedRow);
           }

           ui->adhocip->setText("");

           return;
     }


        if (isusb )
          {

             QMessageBox::critical(this,"","Inactive for USB connections");
              return;
          }



        QString selectedDescription;
        int selectedRow = ui->deviceTable->currentRow();
        if (selectedRow >= 0 && ui->deviceTable->item(selectedRow, 0)) {
            selectedDescription = ui->deviceTable->item(selectedRow, 0)->text();
        } else {
            QMessageBox::critical(this, "", "No device selected in table");
            return;
        }

        daddr = getDevice(selectedDescription);
        if (daddr.isEmpty()) {
            daddr = selectedDescription;
        }

        QString port = getPort(selectedDescription);
        if (port.isEmpty()) {
            port = "5555";
        }


             QString cstring = getadbpath() + " disconnect "+daddr+":"+port ;
             QString command=getadbOutput(cstring);
             logfile (command);
             logfile("disconnect: "+daddr);

             if (selectedRow >= 0 && ui->deviceTable->item(selectedRow, 2)) {
            ui->deviceTable->setItem(selectedRow, 1, new QTableWidgetItem("Disconnected"));
             }



             loadDeviceTable();

    }


    /////////////////////////////////////////////////////////////////////////
    void MainWindow::on_actionAbout_triggered()
    {
        Dialog2 dialog2(this);
        dialog2.setWindowModality(Qt::WindowModal);
        dialog2.setaLabel(program+" "+version);
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



    ////////////////////////////////////////

    void MainWindow::androidRemote()
    {

        QString cstring;
        QString command;
        QString mcpath="";


        QString selectedDescription;
        int selectedRow = ui->deviceTable->currentRow();
        if (selectedRow >= 0 && ui->deviceTable->item(selectedRow, 0)) {
            selectedDescription = ui->deviceTable->item(selectedRow, 0)->text();
        } else {
            QMessageBox::critical(this, "", "No device selected in table");
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


    ////////////////////////////////////////

    void MainWindow::otherRemote()
    {


        QElapsedTimer rtimer;
        int nMilliseconds;
        rtimer.start();



        QString mcpath=filepath;


        mcpath = mcpath+"/userdata/keymaps/";

        QDir dir(mcpath);
        if (!dir.exists()) {
            dir.mkpath(mcpath);
        }

         QString fileName = QFileDialog::getOpenFileName(this,
         "Choose remote xml file", xmldir, tr("Files (*.xml)"));

         if (!fileName.isEmpty() )
         {


             QMessageBox::StandardButton reply;
               reply = QMessageBox::question(this, "Push", fileName+" selected. Continue?",
                   QMessageBox::Yes|QMessageBox::No);
               if (reply == QMessageBox::Yes) {

                   QFileInfo fileInfo(fileName);
                   QString filename(fileInfo.fileName());




                  QFile::copy(fileName, mcpath+"/"+"keyboard.xml");

                   nMilliseconds = rtimer.elapsed();
                   logfile("process time duration: "+ QString::number(nMilliseconds/1000)+ " seconds" );


                   if (QFileInfo::exists(mcpath+"/"+"keyboard.xml"))
                   {

                        QMessageBox::information(this,"","Remote xml installed.");
                   }

                 else {
                   QMessageBox::critical(this,"","Copy failed. See log." );
                   logfile("Copy of "+fileName);
                   logfile("to folder "+mcpath);
                   logfile("failed for "+filename);

                   }



                   }


        }
    }





    void MainWindow::on_actionPush_remote_triggered()
    {

        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
                   return;
        }




           androidRemote();


    }







    ////////////////////////////////////////////////////////////////////////////////
    void MainWindow::on_actionReboot_triggered()
    {


          QString selectedDescription;
          if (!validateDeviceSelection(selectedDescription)) {
           return;
          }

          DeviceRecord device = queryDeviceRecord(selectedDescription);

          if (device.ostype != "0") {
           QMessageBox::critical(this, "Unavailable", "Android devices only.");
           return;
          }



       QMessageBox::StandardButton reply;
         reply = QMessageBox::question(this, "Reboot Device", "Reboot Device?",
             QMessageBox::Yes|QMessageBox::No);
         if (reply == QMessageBox::Yes) {
             isConnected=false;
            logfile("rebooting device");
            rebootDevice(" reboot");
         }

    }



    //////////////////////////////////////////

    void MainWindow::on_actionReboot_bootloader_triggered()
    {


         QString selectedDescription;
         if (!validateDeviceSelection(selectedDescription)) {
            return;
         }

         DeviceRecord device = queryDeviceRecord(selectedDescription);

         if (device.ostype != "0") {
            QMessageBox::critical(this, "Unavailable", "Android devices only.");
            return;
         }




       QMessageBox::StandardButton reply;
         reply = QMessageBox::question(this, "", "Reboot to bootloader?",
             QMessageBox::Yes|QMessageBox::No);
         if (reply == QMessageBox::Yes) {
             isConnected=false;


             logfile("rebooting device recovery");
             rebootDevice(" reboot bootloader");
            }

    }



    ///////////////////////////////////////////////////////////////////////////////////
    void MainWindow::on_actionRecovery_triggered()
    {


            QString selectedDescription;
            if (!validateDeviceSelection(selectedDescription)) {
             return;
            }

            DeviceRecord device = queryDeviceRecord(selectedDescription);

            if (device.ostype != "0") {
             QMessageBox::critical(this, "Unavailable", "Android devices only.");
             return;
            }





       QMessageBox::StandardButton reply;
         reply = QMessageBox::question(this, "", "Reboot to recovery?",
             QMessageBox::Yes|QMessageBox::No);
         if (reply == QMessageBox::Yes) {
             isConnected=false;


             logfile("rebooting device recovery");
             rebootDevice(" reboot recovery");
            }

    }





    /////////////////////////////////////////
    void MainWindow::on_donate_clicked()
    {
        QString link = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=GKZMW456H6E5W";
       QDesktopServices::openUrl(QUrl(link));

    }





    ////////////////////////////////////////
    void MainWindow::on_killServer_clicked()
    {


     if (QMessageBox::question(this, "Disconnect", "Disconnect all devices?", QMessageBox::Cancel | QMessageBox::Ok) == QMessageBox::Cancel)
             {   return;

     }


             QString cstring = getadbpath() + " kill-server";
             QString command=getadbOutput(cstring);
             ui->server_running->setText(adbstr2);
             loadDeviceTable();
             return;







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
             QString olddaddr;
             QString olddescription;

             // Fetch connected devices using adb
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
                   olddaddr = device.daddr;
                   olddescription = device.description;
                   updatecheck = true;
                   qDebug() << device.description << " " << device.pulldir;

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
             dialog.setostype(device.ostype);
             dialog.setdescription(device.description);
             dialog.setdisableroot(device.disableroot);
             if (device.isusb)
                   dialog.setport("");
             else
                   dialog.setport(device.port);
             dialog.setscope(device.scoped);
             dialog.setwsa(device.wsa);
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
             // bool scoped = dialog.scoped();
            // bool wsa = dialog.wsa();
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

             loadDeviceTable();
             }
    }
//////////////////

    ////////////////////////////////////////
    void MainWindow::on_delRecord_clicked()
    {

      QString descrip;
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

    ui->deviceTable->removeRow(selectedRow);

    logfile(descrip + " is deleted");

      }
    }


    //////////////////////////////////////////////////////////////////////
    void MainWindow::cacheButton_other()
    {

        bool oldxml = false;


        QString line1;
        QString line2;
        QString line3;
        QString line4;
        QString line5;
        QString line6;
        QString line7;



    QString mcpath=filepath;

    mcpath = mcpath+"/userdata/";

    if (!QFileInfo::exists(mcpath))
    {
         QMessageBox::information(this,"","userdata folder not found" );
         return;
    }

    logfile("opening cache dialog");

       cacheDialog dialog;

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


             QString str1;
             str1.setNum(buffermode-1);

                  QString filename1 = "advancedsettings.xml";
                  QString filename2 = mcpath+"/"+filename1;

                  if (QFileInfo::exists(filename2))   // does file exist

                  {
                        logfile("advancedsettings.xml exists");
                              QMessageBox::StandardButton reply;
                               reply = QMessageBox::question(this, "XML", "advancedsettings.xml exists. Overwrite?\n(will backup original)",
                                                             QMessageBox::Yes|QMessageBox::No);
                               if (reply == QMessageBox::No) {
                                   logfile("abort xml write");
                                   return;
                                  } else
                                  {
                                  logfile("continue xml write");
                                  logfile("backup advancedsettings.xml");
                                  QFile::rename(filename2, mcpath+"/"+"advancedsettings.old.xml");
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


           }

      }  //end accepted


    } // end  other cache

    //////////////////////////////////////////////////////////////////////
    void MainWindow::cacheButton_android()

    {

      QString selectedDescription;
      if (!validateDeviceSelection(selectedDescription)) {
           return;
      }

      DeviceRecord device = queryDeviceRecord(selectedDescription);

      if (device.ostype != "0") {
           QMessageBox::critical(this, "Unavailable", "Android devices only.");
           return;
      }

        bool oldxml = false;


        QString line1;
        QString line2;
        QString line3;
        QString line4;
        QString line5;
        QString line6;
        QString line7;




    logfile("opening cache dialog");

    if (data_root.isEmpty())
        data_root="/sdcard/";

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
           mcpath=data_root + "kodi_data/" + xbmcpackage+"/files/.kodi";
        else
           mcpath=data_root + "Android/data/" + xbmcpackage+"/files/.kodi";

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




        /*
        <cachemembuffersize> and <readbufferfactor>
         - In v17 <cachemembuffersize> is renamed to  <memorysize>
         and <readbufferfactor> is renamed to <readfactor>.
        In addition, all three buffer related settings in <network>
        are moved out of <network> and
        into a new <cache> parent tag.

        */

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


    } // end  android cache



    ///////////////////////////////////////////
    void MainWindow::on_cacheButton_clicked()

    {


      QString selectedDescription;
      if (!validateDeviceSelection(selectedDescription)) {
               return;
      }

      DeviceRecord device = queryDeviceRecord(selectedDescription);



        if (  (device.ostype != "0") )
         {
               cacheButton_other();
           }

           else

          {
            cacheButton_android();
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



           if (device.ostype != "0")
            {
                otherLog();
           }

           else


          {
            androidLog();
          }




    }

    //////////////////////////////////////////////////
    void MainWindow::androidLog()
    {
          QString selectedDescription;
          if (!validateDeviceSelection(selectedDescription)) {
            return;
          }

          DeviceRecord device = queryDeviceRecord(selectedDescription);

          if (device.ostype != "0") {
            QMessageBox::critical(this, "Unavailable", "Android devices only.");
            return;
          }

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
                  mcpath=device.data_root + "kodi_data/" + xbmcpackage+"/files/.kodi";
            else
                  mcpath=device.data_root + "Android/data/" + xbmcpackage+"/files/.kodi";

          }





          xpath = mcpath+"/temp/";



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


    //////////////////////////////////////////////////
    void MainWindow::otherLog()
    {

        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
            return;
        }

        DeviceRecord device = queryDeviceRecord(selectedDescription);

       // iOS/ATV2	/private/var/mobile/Library/Preferences/kodi.log
       // Linux	$HOME/.kodi/temp/kodi.log
      //   Mac OS X	/Users/<username>/Library/Logs/kodi.log
      //  Windows	%APPDATA%\Kodi\kodi.log


        logfile("opening kodi log");
        oslogDialog oslogdialog;
        oslogdialog.ospassdata(device.ostype);
        oslogdialog.setModal(true);
        oslogdialog.exec();

    }


    ////////////////////////////////////////////////////
    void MainWindow::on_actionView_adbLink_Log_triggered()
    {
        adblogDialog dialog;
        dialog.setModal(true);
        dialog.exec();

    }

    /////////////////////////////////////////////

     void MainWindow::on_stopapp_clicked()

    {

        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
            return;
        }

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        if (device.ostype != "0") {
            QMessageBox::critical(this, "Unavailable", "Android devices only.");
            return;
        }

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

      //      QString busybox;
      //         busybox = apphome+"/adbfiles/busybox";

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


    /////////////////////////////////////////////////////////////////////

    void MainWindow::copyFolder(QString sourceFolder, QString destFolder)
    {

    QDir sourceDir(sourceFolder);
    if(!sourceDir.exists())
    return;

    QDir destDir(destFolder);
    if(!destDir.exists())
    {
    destDir.mkdir(destFolder);
    }

    QStringList files = sourceDir.entryList(QDir::Files);

    for(int i = 0; i< files.count(); i++)
    {
    QString srcName = sourceFolder + "/" + files[i];
    QString destName = destFolder + "/" + files[i];
    QFile::copy(srcName, destName);
    }

    files.clear();

    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    for(int i = 0; i< files.count(); i++)
    {
    QString srcName = sourceFolder + "/" + files[i];
    QString destName = destFolder + "/" + files[i];

    copyFolder(srcName, destName);
    }



    }



    /////////////////////////////////////////////////////////////////////

    void MainWindow::kodiFolder(QString sourceFolder, QString destFolder)
    {


    QDir sourceDir(sourceFolder);
    QDir destDir(destFolder);

    if(!sourceDir.exists())
    return;

    QDir backupfolder(destFolder+".backup");

      if (backupfolder.exists())
      {
         logfile("Kodi folder backup already exists. Delete or rename it.");
         return;
      }



     bool check = destDir.rename(destFolder, destFolder+".backup");

     if (!check)
     {
         logfile("Problem backing up local Kodi folder");
         return;
     }



    if(!destDir.exists())
    {
    destDir.mkdir(destFolder);
    }

    QStringList files = sourceDir.entryList(QDir::Files);
    for(int i = 0; i< files.count(); i++)
    {
    QString srcName = sourceFolder + "/" + files[i];
    QString destName = destFolder + "/" + files[i];
    QFile::copy(srcName, destName);
    }
    files.clear();
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for(int i = 0; i< files.count(); i++)
    {
    QString srcName = sourceFolder + "/" + files[i];
    QString destName = destFolder + "/" + files[i];
    copyFolder(srcName, destName);
    }



    }



    /////////////////////////////////////////////
    void MainWindow::finishedCopy1()
    {


      logfile("Backup finished.");

        QString mstring =  "Local backup";

       for(int i = 0; i < ui->listRunningJobs->count(); ++i)
       {
           QString str = ui->listRunningJobs->item(i)->text();
          if (str==mstring)
              delete ui->listRunningJobs->item(i);
       }


    if (ui->listRunningJobs->count() > 0 )
           {
            ui->progressBar->setHidden(false);
            ui->progressBar->setValue(0);
             }
       else
          {
            ui->progressBar->setHidden(true);
            ui->progressBar->setValue(0);
        }










    QMessageBox::information(this,"","Local backup finished");


    }



    /////////////////////////////////////////////
    void MainWindow::finishedCopy2()
    {

      logfile("Restore finished.");

         // QString mstring =  "Local Kodi restore";
         QString mstring =  "Local restore";
       for(int i = 0; i < ui->listRunningJobs->count(); ++i)
       {
           QString str = ui->listRunningJobs->item(i)->text();
          if (str==mstring)
              delete ui->listRunningJobs->item(i);
       }


       if (ui->listRunningJobs->count() > 0 )
              {
               ui->progressBar->setHidden(false);
               ui->progressBar->setValue(0);
                }
          else
             {
               ui->progressBar->setHidden(true);
               ui->progressBar->setValue(0);
              }

           QMessageBox::information(this,"","Local restore finished");

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

                  if (device.ostype != "0") {
                    QMessageBox::critical(this, "Unavailable", "Android devices only.");
                    return;
                  }


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

        if (device.ostype != "0") {
        QMessageBox::critical(this, "Unavailable", "Android devices only.");
        return;
        }



                bool startstop;
                QString startapp;


                if (QFileInfo::exists(databasedir+"/startapp.json"))
                  startstop = true;
                else
                 startstop = false;

                if (!startstop)
                {
                  QJsonObject obj;
                  obj["startapp"] = "org.xbmc.kodi/org.xbmc.kodi.Xplash";
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

    void MainWindow::on_keypadButton_clicked()
    {



        QString port;
        QString daddr;

        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
            return;
        }


        DeviceRecord device = queryDeviceRecord(selectedDescription);


        if (device.ostype != "0") {
            QMessageBox::critical(this, "", "Android devices only.");
            return;
        }



        if (device.port.isEmpty()) {
            port = "5555";
        }

        daddr = device.daddr+":"+port;


         QString cstring = daddr + " shell input keyevent ";
         keyboardDialog dialog(this);
         dialog.setWindowModality(Qt::WindowModal);
         dialog.setdaddressLabel(cstring);
         dialog.exec();

    }


    ///////////////////////////////////////////////////////


    void MainWindow::on_fmButton_clicked()
    {
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
         QString port;
         QString daddr;
         QString ostypefm("");
         QString fmdaddr("");
         bool iskodi;

         busybox_permissions();


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



         //fmdaddr = ui->deviceTable->item(selectedRow, 1)->text();
         fmdaddr = device.daddr;



         if (device.isusb) {
          port = "";
          daddr = device.daddr;
         } else {
          port = device.port.isEmpty() ? "5555" : device.port;
          daddr = device.daddr + ":" + port;
         }



         if (device.ostype != "0") {
            QMessageBox::critical(nullptr, "Error", "Android devices only");
            return;
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

         QString kp = data_root + filepath;

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
         fmdialog->setdisableroot(device.disableroot);
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


    ///////////////////////////////////////////////
    QString MainWindow::RunLongProcess(QString cstring, QString jobname)
    {

    ui->progressBar->setHidden(false);
    ui->progressBar->setValue(0);
    QString command;
    QString s = description+" "+jobname;
    RunProcessList << s;
    ui->listRunningJobs->addItem(s);
    ui->server_running->setText(s);

    int tsvalue = 4000;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
    timer->start(tsvalue);

     command=getadbOutput(cstring);

     RunProcessList.removeAll(s);

       ui->listRunningJobs->clear();


       for (QList<QString>::iterator it = RunProcessList.begin(); it != RunProcessList.end(); ++it) {
         QString current = *it;
         ui->listRunningJobs->addItem(current);
       }




      if (RunProcessList.count() > 0 )
        {  //activityIcon(true);
          ui->progressBar->setHidden(false);
          ui->progressBar->setValue(0);
         }
      else
        {  //activityIcon(false);
           ui->progressBar->setHidden(true);
           ui->progressBar->setValue(0);
           ui->server_running->setText("");
         }

     return command;
    }


    /////////////////////////////////////////////
    void MainWindow::finishedLocalBackup()
    {

      logfile("Backup finished.");

      QString mstring =  "Local Kodi backup";

     for(int i = 0; i < ui->listRunningJobs->count(); ++i)
     {
         QString str = ui->listRunningJobs->item(i)->text();
        if (str==mstring)
            delete ui->listRunningJobs->item(i);
     }



    }




    //////////////////////////////////////////////

    void MainWindow::updateDeviceRecord()
    {

        QString tempstring;
        QString sqlstatement;
        QSqlQuery query;
       // QString quote = "\"";


         data_root="VVVV";

         tempstring = data_root;

         //tempstring.replace(QString("'"), QString("''"));


       //   sqlstatement = "UPDATE device SET data_root='"+tempstring+"'  WHERE description="+'"'+ui->deviceXox->currentText()+'"';


         logfile(sqlstatement);

          query.exec(sqlstatement);

          if (query.lastError().isValid())
           {
             logfile(sqlstatement);
             logfile("SqLite error:" + query.lastError().text());
             logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
            }


    }


    ////////////////////////////////////////////////////////

    void MainWindow::on_actionMount_system_RO_triggered()
    {


            QString selectedDescription;
            if (!validateDeviceSelection(selectedDescription)) {
             return;
            }

            DeviceRecord device = queryDeviceRecord(selectedDescription);

            if (device.ostype != "0") {
             QMessageBox::critical(this, "Unavailable", "Android devices only.");
             return;
            }


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

        if (device.ostype != "0") {
             QMessageBox::critical(this, "Unavailable", "Android devices only.");
             return;
        }



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

        if (device.ostype != "0") {
                QMessageBox::critical(this, "Unavailable", "Android devices only.");
                return;
        }





       /*

    setprop service.adb.tcp.port 5555
    stop adbd
    start adbd

    setprop service.adb.tcp.port -1
    stop adbd
    start adbd

    */



       // QString tcpstatus = getadb() +   " shell getprop service.adb.tcp.port";
      //   QString tcpstatus = getadb() +   " shell getprop persist.adb.tcp.port 5555";
    // QString tcpstatus = getadb() +   " shell getprop persist.adb.tcp.port -1";
        // QString tcpstatus = getadb() +   " usb";

       tcpipDialog dialog;

         //  QString tcpstatus = getadb() +   " shell getprop service.adb.tcp.port";


       QString tcpstatus = getadb() +   " shell getprop persist.adb.tcp.port 5555";
           QString command=getadbOutput(tcpstatus);

           logfile(command);

           if (command.contains("5555"))
             dialog.settcplabel("ADB/WIFI is enabled");
           else
            dialog.settcplabel("ADB/WIFI is disabled");

           if (port=="")
               port="5555";

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
              int selectedRow = ui->deviceTable->currentRow();
              if (selectedRow >= 0 && ui->deviceTable->item(selectedRow, 0)) {
                selectedDescription = ui->deviceTable->item(selectedRow, 0)->text();
              } else {
                QMessageBox::critical(this, "", "No device selected in table");
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

    ////////////////////////////////

    void MainWindow::on_clearButton_clicked()
    {
        ui->adhocip->setText("");


    }



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

         bool checkversion = doc.object()["checkversion"].toBool();
         bool checkscope = doc.object()["checkscope"].toBool();
         bool scrcpy = doc.object()["scrcpy"].toBool();
         bool startview = doc.object()["startview"].toBool();


         file.close();

         if (checkversion)
             dialog.setversioncheck(true);
         else
             dialog.setversioncheck(false);

         if (checkscope)
             dialog.setscopecheck(true);
         else
             dialog.setscopecheck(false);

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
         dialog.setinstalldir(install);
         dialog.setbackupdir(backup);

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
             obj["checkscope"] = dialog.scopecheck();
             obj["scrcpy"] = dialog.scrcpyargs();

             obj["startview"] = dialog.startview();



             obj["download"] = dialog.downloaddir();
             obj["install"] = dialog.installdir();
             obj["backup"] = dialog.backupdir();

             QJsonDocument doc(obj);

             QFile file(databasedir + "adblink.json");
             file.open(QIODevice::WriteOnly);
             file.write(doc.toJson());
             file.close();


         }
    }

    ////////////////////////////////

    bool MainWindow::getadbPref()

    {






        QString sqlstatement;

        QSqlQuery query;

         QString tmpstr ;

         int ckver;

         bool vercheck;

                 sqlstatement= "SELECT  checkversion FROM preferences";
                 query.exec(sqlstatement);



                         while (query.next()) {
                               tmpstr = query.value(0).toString();
                         }


                           if (query.lastError().isValid())
                            {
                              logfile(sqlstatement);
                              logfile("SqLite error:" + query.lastError().text());
                              logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                             }

          ckver=tmpstr.toInt();

         if (ckver==0)
             vercheck=false;
         else
             vercheck=true;


         return vercheck;

    }


    //////////////////////////////////////

    int  MainWindow::getmsgboxtype()

    {


        QString sqlstatement;

        QSqlQuery query;

         QString tmpstr ;

       int mcheck;

                 sqlstatement= "SELECT  msgboxtype FROM preferences";
                 query.exec(sqlstatement);



                         while (query.next()) {
                               tmpstr = query.value(0).toString();
                         }


                           if (query.lastError().isValid())
                            {
                              logfile(sqlstatement);
                              logfile("SqLite error:" + query.lastError().text());
                              logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                             }




                           mcheck=tmpstr.toInt();




         return mcheck;

    }



    //////////////////////////////////////////

    void MainWindow::updateckversion(int value)
    {

        QString sqlstatement;
        QSqlQuery query;
        QString str;

      str.setNum(value);

        sqlstatement = "UPDATE preferences SET checkversion='"+str+"' ";
        logfile(sqlstatement);
        query.exec(sqlstatement);

         if (query.lastError().isValid())
          {
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
           }




    }



    //////////////////////////////////////////

    void MainWindow::updateMsgboxtype(int value)
    {

        QString sqlstatement;
        QSqlQuery query;
        QString str;

      str.setNum(value);

        sqlstatement = "UPDATE preferences SET msgboxtype='"+str+"'";
        logfile(sqlstatement);
        query.exec(sqlstatement);

         if (query.lastError().isValid())
          {
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
           }




    }



    //////////////////////////////////////////

    void MainWindow::updatedownloaddir(QString str)
    {

        QString sqlstatement;
        QSqlQuery query;


        sqlstatement = "UPDATE preferences SET downloaddir='"+str+"'";
        logfile(sqlstatement);
        query.exec(sqlstatement);

         if (query.lastError().isValid())
          {
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
           }




    }





    //////////////////////////////////////

    QString MainWindow::getdownloadpath()

    {

        return ("");

        QSqlQuery query;
        QString sqlstatement;
       QString downloaddir;

        sqlstatement= "SELECT downloaddir FROM preferences WHERE Id=1";

        query.prepare( sqlstatement );

        if (!query.exec()){
            logfile(sqlstatement);
            logfile("SqLite error:" + query.lastError().text());
            logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
        }
        else
        {
            if( query.next( ) )
             downloaddir = query.value(0).toString();
        }



    return downloaddir;


    }


    void MainWindow::on_actionDown_SPMC_triggered()
    {


        QString link = "http://spmc.semperpax.com/";
        QDesktopServices::openUrl(QUrl(link));


    }





    ////////////////////////////////////////////////

    void MainWindow::on_actionDownload_Kodi_triggered()
    {




        QString link = "https://kodi.tv/download";

        QDesktopServices::openUrl(QUrl(link));



    }




    ///////////////////////////////////////////////

    QString MainWindow::getDaddr(QString descrip)

    {

        QString quote = "\"";
        QString mdaddr;
        QString sqlstatement;
        descrip = quote+descrip+quote;
        QSqlQuery query;



                 sqlstatement= "SELECT daddr FROM device WHERE description=" + descrip;
                 query.exec(sqlstatement);
                 logfile(sqlstatement);


                         while (query.next()) {
                                mdaddr = query.value(0).toString();
                              }


                           if (query.lastError().isValid())
                            {
                              logfile(sqlstatement);
                              logfile("SqLite error:" + query.lastError().text());
                              logfile("SqLite error code:"+ QString::number( query.lastError().number() ));
                             }


    return mdaddr;


    }

    ////////////////////////////////////////////////

    void MainWindow::on_adhocip_returnPressed()
    {
        on_connButton_clicked();
    }




    ///////////////////////////////////////////////////////

    bool MainWindow::xcheck_devices()
    {


     return true;

    }


    //////////////////////////////////////////////
    void MainWindow::open_pref_database()

    {


        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbstring);


        if (!db.open()) {
            QMessageBox::critical(0, qApp->tr("Cannot open database"),
                "Database error:\n"+dbstring
                         , QMessageBox::Cancel);
         logfile("error opening database "+dbstring);

        }



    }



    ////////////////////////////////////////////////////////

    void MainWindow::on_actionReiinstall_Busybox_triggered()
    {


    //    QString busybox;
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
    void MainWindow::on_actionArchitecture_triggered()
    {

    systeminfo();

    }



    ///////////////////////////////////////////////////////
    void MainWindow::systeminfo()
    {
    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
                   return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    if (device.ostype != "0") {
                   QMessageBox::critical(this, "Unavailable", "Android devices only.");
                   return;
    }
       QString android = QString::number(getandroid());
       QString cstring;
       QString archi;

       QString adevice=devicename();
       QString manufact=manufacturer();

       QString scoped;
       QString kbase = "/sdcard/Android/data/";
       QStringList list;


      if (!isScoped())
         scoped = "false";
      else scoped = "true";


       cstring = getadb() + " shell getprop ro.product.cpu.abi";
       archi=getadbOutput(cstring);

       list.append(archi);
       list.append(android);
       list.append(scoped);
       list.append(adevice);
       list.append(manufact);


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

    }






    void MainWindow::on_pushTimers_clicked()
    {
        on_actionSleep_adjust_triggered();
    }



    void MainWindow::on_actionSleep_adjust_triggered()
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

           dialog.setdevicelabel(description);
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


    void MainWindow::on_actiondelthumb_triggered()
    {
            QString selectedDescription;
            if (!validateDeviceSelection(selectedDescription)) {
                return;
            }

            DeviceRecord device = queryDeviceRecord(selectedDescription);

            if (device.ostype != "0") {
                QMessageBox::critical(this, "Unavailable", "Android devices only.");
                return;
            }

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




    void MainWindow::on_actionTest_adb_connection_triggered()
    {

            QString selectedDescription;
            if (!validateDeviceSelection(selectedDescription)) {
              return;
            }

            DeviceRecord device = queryDeviceRecord(selectedDescription);

            if (device.ostype != "0") {
              QMessageBox::critical(this, "Unavailable", "Android devices only.");
              return;
            }


        QJsonObject obj;
        QJsonDocument doc(obj);
        QFile file(databasedir+"adblink.json");
        file.open(QIODevice::ReadOnly);
        doc = QJsonDocument::fromJson(file.readAll());
        obj = doc.object();
        QString dropdown = obj["dropdown"].toString();
        int mcheck=dropdown.toInt();



        logfile("detaching console process");


        if (device.isusb)
             logfile(device.daddr);
         else
             logfile(device.daddr+":"+device.port);




        QString cstring = "";
        QString command ="";
        QString editport = "";


        if (os == 1)

        {


             QString commstr = scriptdir+"/logcat.bat";
             QFile file(commstr);

             if(!file.open(QFile::WriteOnly |
                            QFile::Text))
             {
                  logfile("error creating logcat.bat!");
                  QMessageBox::critical(this,"","Error creating bat file!");
                  return;
             }





             QTextStream out(&file);

             out  << "set PATH=%PATH%;"+adbfiles+";" << endl;

             out  <<  "getadbpath() -s "+ device.daddr + " logcat" << endl;



             file.flush();
             file.close();



             QProcess::startDetached("cmd.exe", QStringList() << "/c" << "start"  << "" << commstr);

        }



         else

           {

            QString commstr = apphome+"logcat.sh";
            QFile::remove(commstr);
            QFile file(commstr);



                     if(!file.open(QFile::WriteOnly))

                 {
                    logfile("error creating console.sh!");
                     QMessageBox::critical(this,"","Error creating command file!");
                     return;
                 }



           QTextStream out(&file);

           out  << "#!/bin/sh" << endl;
           out  <<  getadb()+ " logcat " << endl;
           cstring = getadb()+ " logcat";

                 file.flush();
                 file.close();

           cstring = "chmod 0755 " + commstr ;
           command=getadbOutput(cstring);

            QString shelldir = '"'+apphome+"logcat.sh"+'"';



           if (os == 0)
                {

                if (mcheck==0)
                    cstring = "konsole --workdir="+apphome+ " -e "+apphome+"logcat.sh";
                    else
                    cstring = "/usr/bin/xfce4-terminal --working-directory="+apphome+ " -x "+apphome+"logcat.sh";
                    // cstring = "/usr/bin/gnome-terminal --working-directory="+apphome+ " -x "+apphome+"logcat.sh";

                 }


           if (os == 2)
                {

                if (mcheck==0)
                    cstring = "open -a iTerm.app "+shelldir;
                    else
                    cstring = "open -a Terminal.app "+shelldir;;

                 }


           QProcess::startDetached(cstring);

        }

    }





    ///////////////////////////////////
    void MainWindow::on_actionCreate_kodi_data_triggered()

     {

        QString selectedDescription;
        int selectedRow = ui->deviceTable->currentRow();
        if (selectedRow >= 0 && ui->deviceTable->item(selectedRow, 0)) {
           selectedDescription = ui->deviceTable->item(selectedRow, 0)->text();
        } else {
           QMessageBox::critical(this, "", "No device selected in table");
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
           reply = QMessageBox::question(this, "Stop Kodi", "Cannot create path while Kodi is running.\n Stop "+xbmcpackage+" on device?"  ,
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

       void MainWindow::on_doConsole_clicked()
       {


           QJsonObject obj;
           QJsonDocument doc(obj);
           QFile file(databasedir+"adblink.json");
           file.open(QIODevice::ReadOnly);
           doc = QJsonDocument::fromJson(file.readAll());
           obj = doc.object();
           QString dropdown = obj["dropdown"].toString();
           int mcheck=dropdown.toInt();

           // set PATH=%PATH%;C:\xampp\php

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

                       out  << "set PATH=%PATH%;"+adbfiles+";"<< endl;



                       file.flush();
                       file.close();


     //  if (mcheck == 0)
       //    QProcess::startDetached("cmd.exe", {"/k", commstr});
      // else
      //     QProcess::startDetached("wt -d c:\\ cmd /k "+commstr);

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


       void MainWindow::on_adbshellButton_clicked()
       {

              QString port;
              QString daddr;

              QString selectedDescription;
              if (!validateDeviceSelection(selectedDescription)) {
                 return;
              }


              // Query entire device record
              DeviceRecord device = queryDeviceRecord(selectedDescription);


              if (device.ostype != "0") {
                 QMessageBox::critical(this, "", "ADB shell is for Android devices only.");
                 return;
              }


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



                 if (ui->adhocip->text().isEmpty())
                 {

                    qDebug() << daddr;
                    cstring = getadbpath() + " -s " + daddr + " shell ";
                    out << cstring << endl;

                 }
                 else
                 {
                    out << getadbpath() + " -s " + ui->adhocip->text() + " shell " << endl;

                 }


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

       void MainWindow::on_scpyButton_clicked()
       {
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


              if (device.ostype != "0") {
                 QMessageBox::critical(this, "", "Scrcpy is for Android devices only.");
                 return;
              }



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


    void MainWindow::on_pushButton_clicked()
    {



    }




    /////////////////////////////////////////////////////////////////////////
    void MainWindow::on_connWSA_clicked()
    {




        QString cstring;
        QString command;
        QString s;


        if (!ui->adhocip->text().isEmpty())
        {

                         QString adhocIPText = ui->adhocip->text();

                         int colonIndex = adhocIPText.indexOf(':');
                         if (colonIndex != -1) {
                            daddr = adhocIPText.left(colonIndex);
                            port = adhocIPText.mid(colonIndex + 1);
                         } else {
                            daddr = adhocIPText;
                            port = "5555";
                         }

                         qDebug() << "daddr:" << daddr;
                         qDebug() << "port:" << port;


                         cstring = getadbpath() + " connect "+daddr+":"+port;
                         logfile(cstring);
                         command=connectadb(cstring);

                         if (command.contains("connected to"))
                         {
                            isConnected=true;
                            default_device_values();

                            logfile("Connected to "+daddr);
                            logfile ("Android version: "+ s.setNum(getandroid()));
                         }

                         else {
                            isConnected=false;
                            logfile("Unable to connect to: "+daddr+":"+port);
                            QMessageBox::critical(this,"","Unable to connect to: "+daddr+":"+port);
                         }

                         return;


        }


        daddr="127.0.0.1";
        port= "58526";

        cstring = getadbpath() + " connect "+daddr+":"+port;
        command=getadbOutput(cstring);


        if (command.contains("connected to"))
        {   isConnected=true;

            logfile("Connected to "+daddr+":"+port);
        }

        else
           {
            isConnected=false;
            logfile("Unable to connect to: "+daddr+":"+port);
            QMessageBox::critical(this,"","Can't connect to "+daddr);
           }


    }


    //////////////////////////////////////////////////////////

       void MainWindow::dos_shell()

       {


           QString programName = QCoreApplication::applicationName();


           if (daddr=="127.0.0.1")
            port= "58526";


           if (isusb)
                logfile(daddr);
            else
                logfile(daddr+":"+port);


              QString editport = "";
              QString sernum = "";

                if (!isusb)
                editport=":"+port;

                sernum = " -s "+daddr+editport+" ";






                           QString commstr = scriptdir+"/shell.bat";
                           QFile file(commstr);

                               if(!file.open(QFile::WriteOnly |
                                             QFile::Text))
                               {
                                   logfile("error creating shell.bat!");
                                   QMessageBox::critical(this,"","Error creating bat file!");
                                   return;
                               }


                               QTextStream out(&file);


                               out  <<  "echo off"  << endl;


                               out  << "set PATH=%PATH%;"+adbfiles+";" << endl;

                               out  <<  "adb.exe "+ sernum + " shell"  << endl;



                               file.flush();
                               file.close();


                               QProcess::startDetached("cmd.exe", QStringList() << "/c" << "start"  << "" << commstr);


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




////////////////////////////////////////////
void MainWindow::on_backupButton_clicked()

{




    QString selectedDescription;
    int selectedRow = ui->deviceTable->currentRow();
    if (selectedRow >= 0 && ui->deviceTable->item(selectedRow, 0)) {
         selectedDescription = ui->deviceTable->item(selectedRow, 0)->text();
    } else {
         QMessageBox::critical(this, "", "No device selected in table");
         return;
    }




      DeviceRecord device = queryDeviceRecord(selectedDescription);


      if (device.ostype != "0") {
         QMessageBox::critical(this, "Unavailable", "Android devices only.");
         return;
      }

      else {
         backupAndroid();
         }






}

////////////////////////////////////////////////

void MainWindow::backupAndroid() {

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
           return;
        }

        // Use databasedir (unchanged from original)
        QString backup = readBackup(databasedir);



        if (!device.isusb) {

           if (device.port.isEmpty())
                  port = "5555";

           editport = ":" + port;

        }

        cstring = getadb() + " shell /data/local/tmp/adblink/busybox find /storage -type d -maxdepth 1";

        // cstring = getadbpath() + device.daddr + editport + " shell /data/local/tmp/adblink/busybox find /storage -type d -maxdepth 1";

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
               list[i] == NULL) {
                  list.removeAt(i);
                  i--;
           }
        }

        n_data_root = "/sdcard";
        list.insert(0, "/sdcard");

        if (list.count() > 1) {
           restDialog dialog(this);
           dialog.setWindowModality(Qt::WindowModal);
           dialog.setWindowTitle("Backup");
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

        if (device.scoped) {
           mcpath = n_data_root + "kodi_data/" + device.xbmcpackage;
           kbase = n_data_root + "kodi_data/";
        } else {
           mcpath = n_data_root + "Android/data/" + device.xbmcpackage;
           kbase = n_data_root + "Android/data/";
        }

        cstring = getadb() + " shell ls " + mcpath + "/files/.kodi";
        if (!getreturncode(cstring)) {
           QMessageBox::critical(this, "", "Kodi's files not found at " + mcpath);
           logfile("Backup: kodi's files not found at " + mcpath);
           return;
        }

        QDir backupDir(backup);
        QString dir = QFileDialog::getExistingDirectory(this, "Choose Backup Destination",
                                                        backupDir.absolutePath(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

        if (!dir.isEmpty()) {
           QMessageBox::StandardButton reply;
           reply = QMessageBox::question(this, "Backup", "backup to " + dir,
                                         QMessageBox::Yes | QMessageBox::No);
           if (reply == QMessageBox::Yes) {
                  logfile("backup function started");
                  mcpath = mcpath + "/";
                  dir = dir + "/";
                  if (os == 1) {
                    dir.replace("/", "\\");
                  }

                  cstring = getadb() + " pull " + mcpath + "files/.kodi/. " + '"' + dir + '"';
                  logfile(cstring);

                  command = RunLongProcess(cstring, "backup running");
                  logfile("backup: " + cstring);

                  if (QDir(dir + "userdata").exists()) {
                    writeBackup(dir);
                    QMessageBox::information(this, "", "Backup complete");
                  } else {
                    logfile("backup: " + command);
                    QMessageBox::critical(this, "", "Backup Failed. See Log.");
                    return;
                  }
           }
        }

        logfile("Backup complete");
}


/////////////////////////////////////////////

void MainWindow::on_restoreButton_clicked() {


   QString selectedDescription;
   if (!validateDeviceSelection(selectedDescription)) {
      return;
   }




   // Query entire device record
   DeviceRecord device = queryDeviceRecord(selectedDescription);

   // Check if device is Android
   if (device.ostype != "0") {
      QMessageBox::critical(this, "Unavailable", "Android devices only.");
      return;
   }




   if (!is_package(device.xbmcpackage)) {
      QMessageBox::critical(this, "", device.xbmcpackage + " not installed");
      return;
   }

   QString cstring;
   QString command;
   QString n_data_root;
   QString mcpath;
   QString xbmcpath;
   QString kbase;
   bool xbmc_env = false;

   // Check for xbmc_env.properties
   cstring = getadb() + " shell ls /sdcard/xbmc_env.properties";
   if (getreturncode(cstring)) {
      cstring = getadb() + " shell cat /sdcard/xbmc_env.properties";
      command = getadbOutput(cstring);
      command.replace(QRegExp("[\r\n]"), "");

      int startIndex = command.indexOf("=") + 1;
      int endIndex = command.indexOf(".kodi") + 5;
      xbmcpath = command.mid(startIndex, endIndex - startIndex);

      QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "xbmc properties", "xbmc_env.properties file found.\nUse its values?",
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
   cstring = getadb() + " shell ps | grep " + device.xbmcpackage;
   command = getadbOutput(cstring);
   if (command.contains(device.xbmcpackage)) {
      QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Stop Kodi", "Cannot restore while Kodi is running.\n Stop " + device.xbmcpackage + " on device?",
                                    QMessageBox::Yes | QMessageBox::No);
      if (reply == QMessageBox::Yes) {
          cstring = getadb() + " shell am force-stop " + device.xbmcpackage;
          command = getadbOutput(cstring);
          logfile(command);
      } else {
          logfile(device.xbmcpackage + " running. Restore failed");
          return;
      }
   }

   // Determine storage root and path if no xbmc_env.properties
   if (!xbmc_env) {
      cstring = getadb() + " shell /data/local/tmp/adblink/busybox find /storage -type d -maxdepth 1";
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
              list[i] == NULL) {
                    list.removeAt(i);
                    i--;
          }
      }

      list.insert(0, "/sdcard");

      if (list.count() > 1) {
          restDialog dialog(this);
          dialog.setWindowModality(Qt::WindowModal);
          dialog.setWindowTitle("Restore");
          dialog.setadb_restore(list);
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

      // Path logic with wsa and scoped
      if (device.wsa) {
          mcpath = n_data_root + "wsa_data/" + device.xbmcpackage;
          kbase = n_data_root + "wsa_data/";
      } else if (device.scoped) {
          mcpath = n_data_root + "kodi_data/" + device.xbmcpackage;
          kbase = n_data_root + "kodi_data/";
          cstring = getadb() + " shell appops set --uid " + device.xbmcpackage + " MANAGE_EXTERNAL_STORAGE allow";
          if (!getreturncode(cstring)) {
                    QMessageBox::critical(this, "", "Error setting Kodi permissions");
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
   QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Backup Folder"),
                                                   backupDir.absolutePath(),
                                                   QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
   if (dir.isEmpty()) {
      return;
   }

   // Validate backup
   if (!QDir(dir + "/userdata").exists()) {
      QMessageBox::critical(this, "", "Invalid backup. No userdata folder.");
      return;
   }
   if (!QDir(dir + "/addons").exists()) {
      QMessageBox::critical(this, "", "Invalid backup. addons folder not found.");
      return;
   }

   // Confirm restore
   QMessageBox::StandardButton reply;
   reply = QMessageBox::question(this, "Restore", "Restore this backup? This will overwrite existing Kodi data.",
                                 QMessageBox::Yes | QMessageBox::No);
   if (reply == QMessageBox::No) {
      return;
   }

   logfile("Restoring Android Kodi");

   // Prepare target directory
   cstring = getadb() + " shell rm -r " + mcpath;
   command = RunLongProcess(cstring, "preparing target");
   logfile(command);

   cstring = getadb() + " shell ls " + mcpath;
   command = getadbOutput(cstring);

   if (command.contains("No such file or directory")) {
      cstring = getadb() + " shell mkdir -p " + mcpath + "/files/.kodi";
      command = getadbOutput(cstring);
      logfile(command);
      QString errorp = command;
      cstring = getadb() + " shell ls " + mcpath + "/files/.kodi";
      command = getadbOutput(cstring);

      if (command.contains("No such file or directory")) {
          QMessageBox::critical(this, "", "Error creating restore point");
          logfile("Restore error: " + errorp);
          return;
      }
   }

   // Perform restore
   dir = dir + "/.";

   //  cstring = getadb() + " push \"" + dir + "\" " + mcpath + "/files/.kodi/";


   command = getadbOutput(cstring);

  cstring = getadb() + " push \"" + dir + "\" " + mcpath + "/files/.kodi/";

   command = RunLongProcess(cstring, "restore running");
   logfile("Restore: " + cstring);

   // Check restore success
   if (command.contains("bytes")) {
      cstring = getadb() + " shell rm /sdcard/xbmc_env.properties";
      command = getadbOutput(cstring);

      if (device.scoped) {
          cstring = getadb() + " shell echo xbmc.data=" + mcpath + "/files > /sdcard/xbmc_env.properties";
          command = getadbOutput(cstring);
          logfile("create /sdcard/xbmc_env.properties");
          logfile(command);
      } else {
          if (n_data_root != "/sdcard/") {
                    cstring = getadb() + " shell echo xbmc.data=" + mcpath + "/files > /sdcard/xbmc_env.properties";
                    command = getadbOutput(cstring);
                    logfile("create /sdcard/xbmc_env.properties");
                    logfile(command);
          }
      }

      writeBackup(dir);
      QMessageBox::information(this, "", "Restore complete");
   } else {
      QMessageBox::critical(this, "", "Restore Failed. See log.");
      logfile(cstring);
      logfile(command);
   }

   logfile("Restore complete");
}


//////////////////////////////////
void MainWindow::restoreAndroid()
//////////////////////////////////

{
   QString selectedDescription;
   if (!validateDeviceSelection(selectedDescription)) {
      return;
   }

   DeviceRecord device = queryDeviceRecord(selectedDescription);

   if (device.ostype != "0") {
      QMessageBox::critical(this, "Unavailable", "Android devices only.");
      return;
   }




if (!is_package(device.xbmcpackage))
   {

    QMessageBox::critical(this,"",device.xbmcpackage+" not installed");
    return;
    }

QString cstring;
QString command;
QString n_data_root;
QString mcpath;
QString xbmcpath;
QString kbase;




bool xbmc_env=false;

cstring = getadb() + " shell ls /sdcard/xbmc_env.properties";
if(getreturncode(cstring))
{

    cstring = getadb() + " shell cat /sdcard/xbmc_env.properties";
    command=getadbOutput(cstring);
    command.replace(QRegExp("[\r\n]"), "");
    xbmcpath = command.mid(command.indexOf("xbmc.data=") + 10);
    xbmcpath=xbmcpath+"/.kodi";


    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "xbmc properties", "xbmc_env.properties file found.\n"+xbmcpath+"\nUse these values for restore?"  ,
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {

         xbmc_env=true;
          mcpath=xbmcpath;

    }


}




//int androidRelease=getandroid();


QString backup = readBackup(databasedir);


cstring = cstring = getadb() +  " shell ps | grep "+device.xbmcpackage;

 command=getadbOutput(cstring);

if (command.contains(device.xbmcpackage))
  {


   QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Stop Kodi", "Cannot restore while Kodi is running.\n Stop "+xbmcpackage+" on device?"  ,
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes)
      {


      QString cstring = getadb() + " shell am force-stop "+device.xbmcpackage;
      QString command=getadbOutput(cstring);
      logfile(command);
   }


      else {


    logfile(device.xbmcpackage+" running. Restore failed");
    return;
}




}

if (!xbmc_env)

{

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





list.insert(0, "/sdcard");

if( list.count() > 1)
{
  restDialog dialog(this);
  dialog.setWindowModality(Qt::WindowModal);
  dialog.setWindowTitle("Restore");
  dialog.setadb_restore(list);


if (dialog.exec() == QDialog::Accepted)
  {

    n_data_root = dialog.restore_data_root();

  }

   else return;



}


if (n_data_root.isEmpty())
n_data_root = "sdcard";



if(!n_data_root.startsWith("/"))
      n_data_root.prepend("/");

   if(!n_data_root.endsWith("/"))
      n_data_root.append("/") ;



if (isScoped()) {
    mcpath=n_data_root + "kodi_data/" + device.xbmcpackage;
    kbase = n_data_root + "kodi_data/";

    cstring = getadb()+ " shell appops set --uid "+  device.xbmcpackage +" MANAGE_EXTERNAL_STORAGE allow";
    if (!getreturncode(cstring))
    { QMessageBox::critical(this, "", "Error setting Kodi permissions");
    return;
    }



} else {
    mcpath=n_data_root + "Android/data/" + device.xbmcpackage;
    kbase = n_data_root + "Android/data/";
}


} // end_xbmc_env





QDir backupDir(backup);
QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Backup Folder"),
                                                   backupDir.absolutePath(),
                                                   QFileDialog::ShowDirsOnly
   | QFileDialog::DontResolveSymlinks);

if (dir.isEmpty()) {
  return;
}




if (!QDir(dir+"/userdata").exists() )

{


    QMessageBox::critical(0,"","Invalid backup. No userdata folder." );



    return;
}




if (!QDir(dir+"/addons").exists() )
{
 QMessageBox::critical(0,"","Invalid backup. addons folder not found." );
 return;
}





if (dir.isEmpty() )
return;



QMessageBox::StandardButton reply;
reply = QMessageBox::question(this, "Restore", "Restore this backup? This will overwrite existing Kodi data.",
                             QMessageBox::Yes|QMessageBox::No);
if (reply == QMessageBox::No)
return;



   logfile("Restoring Android Kodi");




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

       //qDebug() << command;

       if (command.contains("No such file or directory"))
        {
           QMessageBox::critical(this,"","Error creating restore point");
           logfile("Restore error:"+ errorp);
           return;
         }

    } // nuke existing





   dir=dir+"/.";

   cstring = getadb() + " push "+'"'+dir+'"'+ " "+mcpath+"/files/.kodi/";


   command=RunLongProcess(cstring,"Restore");


if (command.contains("bytes"))

   {


    cstring = getadb() + " shell rm /sdcard/xbmc_env.properties";
    command=getadbOutput(cstring);




      if (isScoped())
         {
           cstring = getadb() + " shell echo xbmc.data="+mcpath+"/files > /sdcard/xbmc_env.properties";
           command=getadbOutput(cstring);
           logfile("create /sdcard/xbmc_env.properties");
           logfile(command);
           }
      else
         {
           if (n_data_root != "/sdcard/")
            {
              cstring = getadb() + " shell echo xbmc.data="+mcpath+"/files > /sdcard/xbmc_env.properties";
              command=getadbOutput(cstring);
              logfile("create /sdcard/xbmc_env.properties");
              logfile(command);
            }

           }


       writeBackup(dir);

         QMessageBox::information(this,"","Restore complete");

}

  else

        {
           QMessageBox::critical(this,"","Restore Failed. See log.");
           logfile(cstring);
           logfile(command);
        }



}




///////////////////////////////////////////
void MainWindow::restoreOther()
{


    QMessageBox::critical(this,"","Restore is for Android devices only.");
    return;


}





//////////////////////////////////////////////
void MainWindow::on_mvdataButton_clicked()
{

    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
           return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    if (device.ostype != "0") {
           QMessageBox::critical(this, "Unavailable", "Android devices only.");
           return;
    }





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
           reply = QMessageBox::question(this, "Stop Kodi", "Cannot move data while Kodi is running.\n Stop "+xbmcpackage+" on device?"  ,
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


void MainWindow::on_startapp_clicked()

{

    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
     return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    if (device.ostype != "0") {
     QMessageBox::critical(this, "Unavailable", "Android devices only.");
     return;
    }


    bool startstop;
    QString startapp;


    if (QFileInfo::exists(databasedir+"/startapp.json"))
     startstop = true;
    else
     startstop = false;

    if (!startstop)
    {
     QJsonObject obj;
     obj["startapp"] = "org.xbmc.kodi/org.xbmc.kodi.Xplash";
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


void MainWindow::on_actionSplash_Screen_triggered()
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





void MainWindow::on_actionWSA_triggered()
{

 daddr="127.0.0.1";
 port="58526";

 on_connButton_clicked();

}


void MainWindow::on_actionConnect_WSA_triggered()
{

 daddr="127.0.0.1";

 on_connWSA_clicked();
}



void MainWindow::on_actionSet_Kodi_permissions_triggered()
{

 QString selectedDescription;
 if (!validateDeviceSelection(selectedDescription)) {
            return;
 }

 DeviceRecord device = queryDeviceRecord(selectedDescription);

 if (device.ostype != "0") {
            QMessageBox::critical(this, "Unavailable", "Android devices only.");
            return;
 }



 QString flag;

 QString cstring;


 setpDialog dialog(this);
 dialog.setWindowModality(Qt::WindowModal);

 dialog.setpname(device.xbmcpackage);



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

 if (device.ostype != "0") {
            QMessageBox::critical(this, "Unavailable", "Android devices only.");
            return;
 }
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


 /*


getadbpath() shell 'dumpsys OVRRemoteService | grep Battery' // controllers
adb shell 'dumpsys CompanionService | grep Battery' // headset


getadbpath() shell setprop debug.oculus.guardian_pause 0  // disable
getadbpath() shell setprop debug.oculus.guardian_pause 1 //  enable

getadbpath() shell settings put system screen_off_timeout 7200000
// default 86400000
getadbpath() shell svc power stayon true

 // svc power stayon [true|false|usb|ac|wireless]

Turn off sensor:
shell am broadcast -a com.oculus.vrpowermanager.prox_close

Turn on sensor:
shell
am broadcast -a com.oculus.vrpowermanager.automation_disable


getadbpath() shell setprop debug.oculus.refreshrate NNN

Quest 2 supports 60, 72, 80, 90 and 120)

getadbpath() shell setprop debug.oculus.gpuLevel 2
getadbpath() shell setprop debug.oculus.cpuLevel 2






command to turn on full rate capture for Quest 2:

getadbpath() shell setprop debug.oculus.fullRateCapture 1




// Disable dynamic FFR
getadbpath() shell setprop debug.oculus.foveation.dynamic 0

// Set FFR Level between 0 - 4 (Higher = Better Performance)
getadbpath() shell setprop debug.oculus.foveation.level 4



480 (852×480 pixels)
720 (1280×720 pixels
1080 (1920×1080 pixels)

getadbpath() shell setprop debug.oculus.capture.width [value]
getadbpath() shell setprop debug.oculus.capture.height [value]




command to disable chromatic aberration (correction):

//0 for disabling, 1 for enabling (default)
getadbpath() shell setprop debug.oculus.forceChroma 0

getadbpath() shell setprop debug.oculus.fullRateCapture 1

getadbpath() shell setprop debug.oculus.eyeFovDown 49
getadbpath() shell setprop debug.oculus.eyeFovUp 48
getadbpath() shell setprop debug.oculus.eyeFovOutward 50
getadbpath() shell setprop debug.oculus.eyeFovInward 50


How to enable Link through getadbpath()

getadbpath() shell am start "xrstreamingclient://?launch_location=ODH&alink=true&adaptiveSrcLatencyMs=100&maxAdaptiveSrcLatencyMs=400&posePerSecond=500&sessionId=ODH"
How to disable Link through ADB

getadbpath() shell am force-stop com.oculus.xrstreamingclient
How to enable Air Link through ADB

getadbpath() shell am broadcast -a "com.oculus.systemux.action.TOGGLE_AIRLINK" --ez enable_airlink 1
How to disable Air Link through ADB

getadbpath() shell am broadcast -a "com.oculus.systemux.action.TOGGLE_AIRLINK" --ez enable_airlink 0

Start Link (button 1):
getadbpath() shell am start -S com.oculus.xrstreamingclient/.MainActivity

Stop Link (button 2):
getadbpath() shell am force-stop com.oculus.xrstreamingclient



*/

 QString selectedDescription;
 if (!validateDeviceSelection(selectedDescription)) {
            return;
 }

 DeviceRecord device = queryDeviceRecord(selectedDescription);

 if (device.ostype != "0") {
            QMessageBox::critical(this, "Unavailable", "Android devices only.");
            return;
 }

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







void MainWindow::on_test_clicked()
{


 QString cstring;
 QString command;
 QString mcpath;

 QString selectedDescription;
 if (!validateDeviceSelection(selectedDescription)) {
           return;
 }

 DeviceRecord device = queryDeviceRecord(selectedDescription);

 if (device.ostype != "0") {
           QMessageBox::critical(this, "Unavailable", "Android devices only.");
           return;
 }
 cstring = getadb() + " shell ls /sdcard/xbmc_env.properties";
 if(getreturncode(cstring))
 {  cstring = getadb() + " shell cat /sdcard/xbmc_env.properties";
           command=getadbOutput(cstring);
           command.replace(QRegExp("[\r\n]"), "");
           mcpath = command.mid(command.indexOf("xbmc.data=") + 10);
          // mcpath=mcpath+"/.kodi";
 }


 qDebug() << mcpath;



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
 on_keypadButton_clicked();
}

void MainWindow::on_ascend_clicked()
{
 QString selectedDevice;
 if (ui->deviceTable->currentItem() && ui->deviceTable->currentRow() >= 0) {
           selectedDevice = ui->deviceTable->item(ui->deviceTable->currentRow(), 0)->text();
 }

 // Sort by Device (description) or Status based on radio button
 int sortColumn = ui->dsort->isChecked() ? 0 : (ui->csort->isChecked() ? 2 : 0); // Default to Device if neither is checked
 ui->deviceTable->sortItems(sortColumn, Qt::AscendingOrder);

 // Reselect the previously selected row
 if (!selectedDevice.isEmpty()) {
           for (int row = 0; row < ui->deviceTable->rowCount(); ++row) {
              if (ui->deviceTable->item(row, 0)->text() == selectedDevice) {
               ui->deviceTable->clearSelection(); // Clear previous selection
               ui->deviceTable->setCurrentCell(row, 0); // Set active cell
               ui->deviceTable->selectRow(row); // Highlight entire row
               ui->deviceTable->setFocus(); // Ensure table has focus
               break;
              }
           }
 }
}

/////////////////////////////////

void MainWindow::on_descend_clicked()
{
 QString selectedDevice;
 if (ui->deviceTable->currentItem() && ui->deviceTable->currentRow() >= 0) {
           selectedDevice = ui->deviceTable->item(ui->deviceTable->currentRow(), 0)->text();
 }

 // Sort by Device (description) or Status based on radio button
 int sortColumn = ui->dsort->isChecked() ? 0 : (ui->csort->isChecked() ? 2 : 0); // Default to Device if neither is checked
 ui->deviceTable->sortItems(sortColumn, Qt::DescendingOrder);

 // Reselect the previously selected row
 if (!selectedDevice.isEmpty()) {
           for (int row = 0; row < ui->deviceTable->rowCount(); ++row) {
              if (ui->deviceTable->item(row, 0)->text() == selectedDevice) {
               ui->deviceTable->clearSelection(); // Clear previous selection
               ui->deviceTable->setCurrentCell(row, 0); // Set active cell
               ui->deviceTable->selectRow(row); // Highlight entire row
               ui->deviceTable->setFocus(); // Ensure table has focus
               break;
              }
           }
 }
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

 // Apply fallbacks from getRecord
 if (record.daddr.isEmpty()) {
               record.daddr = description;
 }
 if (record.xbmcpackage.isEmpty()) {
               record.xbmcpackage = "org.xbmc.kodi";
 }



 bool ok;
 int ostypeInt = record.ostype.toInt(&ok);
 if (!ok || ostypeInt > 3) {
               record.ostype = "0";
 }
 if (sldir.isEmpty()) {
               sldir = hdir;
 }
 if (pushdir.isEmpty()) {
               pushdir = hdir;
 }
 if (checkversion == 0) {
               versioncheck = false;
 } else {
               versioncheck = true;
 }

 return record;
}


bool MainWindow::validateDeviceSelection(QString& selectedDescription) {
 // Check if any device is connected in deviceTable
 bool hasConnectedDevice = false;
 for (int i = 0; i < ui->deviceTable->rowCount(); ++i) {
               if (ui->deviceTable->item(i, 2) &&
                   (ui->deviceTable->item(i, 2)->text() == "Connected" ||
                    ui->deviceTable->item(i, 2)->text() == "USB")) {
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
     ui->deviceTable->item(selectedRow, 2)->text() != "Connected" &&
     ui->deviceTable->item(selectedRow, 2)->text() != "USB") {
               QMessageBox::critical(this, "", "Selected device is not connected");
               return false;
 }

 return true;
}



void MainWindow::on_actionSwitch_View_triggered()
{

 if (ui->stackedWidget->currentIndex() == 0) {

               QMessageBox::StandardButton reply;
               reply = QMessageBox::question(this, "Kodi", "Disable Kodi functions?",
                                             QMessageBox::Yes | QMessageBox::No);
               if (reply == QMessageBox::No) {
              return;
               }

               ui->stackedWidget->setCurrentIndex(1);
               ui->menuKodi->menuAction()->setVisible(false);
 }


 else {

               QMessageBox::StandardButton reply;
               reply = QMessageBox::question(this, "Kodi", "Enable Kodi functions?",
                                             QMessageBox::Yes | QMessageBox::No);
               if (reply == QMessageBox::No) {
              return;
               }

               ui->stackedWidget->setCurrentIndex(0);
               ui->menuKodi->menuAction()->setVisible(true);
 }

}





void MainWindow::on_newRecord_clicked()
{
      dataentry(true);
}



/////////////////////////////////////////////

void MainWindow::on_editRecord_clicked()
{
      dataentry(false);


}

void MainWindow::on_screencap2_clicked()
{

      screenCap();
}

void MainWindow::on_screencap1_clicked()
{

      screenCap();

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



void MainWindow::onApplicationQuit() {
      QJsonObject obj;
      QJsonDocument doc(obj);
      QFile file(databasedir + "adblink.json");
      if (!file.open(QIODevice::ReadOnly)) {
               qWarning() << "Cannot open file for reading:" << file.fileName();
               return;
      }
      doc = QJsonDocument::fromJson(file.readAll());
      file.close();
      if (doc.isNull()) {
               qWarning() << "Invalid JSON format in" << file.fileName();
               return;
      }
      obj = doc.object();
      obj["sortpref"] = ui->dsort->isChecked() ? false : true; // Set false if dsort is checked, true otherwise
      doc.setObject(obj);
      if (!file.open(QIODevice::WriteOnly)) {
               qWarning() << "Cannot open file for writing:" << file.fileName();
               return;
      }
      file.write(doc.toJson());
      file.close();
}
void MainWindow::loadDeviceTable()
{
      QString sqlstatement;
      QSqlQuery query;

      ui->deviceTable->clear();
      ui->deviceTable->setColumnCount(3); // Set column count to 3
      ui->deviceTable->setHorizontalHeaderLabels(QStringList() << "Device" << "IP" << "Status"); // Label middle column as IP
      ui->deviceTable->setSelectionBehavior(QAbstractItemView::SelectRows);

      sqlstatement = "SELECT description, daddr, ostype, isusb FROM device";
      query.exec(sqlstatement);
      int row = 0;
      ui->deviceTable->setRowCount(0);
      while (query.next()) {
               ui->deviceTable->insertRow(row);
               // Set Device (description)
               ui->deviceTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString())); // Device
               // Check if isusb is true
               bool isUsb = query.value(3).toBool();
               // Set IP to "N/A" if isusb is true, otherwise use daddr or "N/A" if empty
               QString ip = isUsb ? "N/A" : (query.value(1).toString().isEmpty() ? "N/A" : query.value(1).toString());
               ui->deviceTable->setItem(row, 1, new QTableWidgetItem(ip)); // IP
               // Set Status to "USB" if isusb is true, otherwise based on ostype
               QString status = isUsb ? "USB" : (query.value(2).toString() != "0" ? "Local" : "Disconnected");
               ui->deviceTable->setItem(row, 2, new QTableWidgetItem(status)); // Status
               row++;
      }

      ui->dsort->setChecked(true);
      ui->deviceTable->sortItems(0, Qt::AscendingOrder);
}
