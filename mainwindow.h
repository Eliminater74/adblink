#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QMessageBox>
#include "usbfiledialog.h"
#include <QTextEdit>

#include "devicerecord.h"

namespace Ui {
class MainWindow;
}




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


DeviceRecord queryDeviceRecord(const QString& description);

private slots:


    bool validateIPAddress(const QString& ipAddress);

    bool renameColumnIfNotRenamed(const QString& oldColumnName, const QString& newColumnName);

     void onApplicationQuit();

     void serverlabel();

    bool validateDeviceSelection(QString& selectedDescription);

    void loadDeviceTable();

    void handleFilemanagerFinished();

    void onReqCompleted();

    void adhocip();

    void on_actionAbout_triggered();

    void on_connButton_clicked();



    void on_sideload_Button_clicked();

    void on_disButton_clicked();

    void on_actionQuit_triggered();

    void on_uninstall_Button_clicked();

    bool installAPK(QString filename);

    void on_actionHelp_triggered();

    void on_backupButton_clicked();

    void on_restoreButton_clicked();
    void dos_shell();






    void on_adbshellButton_clicked();







    void delay(int secs);



   void busybox_permissions();

   void createTables();





   QString getadb();

  QString manufacturer();
  QString devicename();

   int getandroid();
   int getperms(QString dir);
   bool isScoped();




   void writeBackup (QString dir);

   QString readBackup (QString databasedir);

   QString checkslash (QString qpath);


   void writeInstall (QString dir);

   QString readInstall (QString databasedir);

   void TimerEvent();
   void delayTimer(int rdelay);


   QString RunLongProcess(QString cstring, QString jobname);



   void usbbuttons(bool isusb);

   void rotate_logfile();

   void logfile2(QString line);

   void rebootDevice(QString reboot);

   bool mount_system(QString mnt);

   bool fileExists(QString path);

   bool is_su();

   bool is_busybox();


     QString strip (QString str);

   void kill_server();

   bool start_server();



   bool is_package(QString package);





    void androidLog();



    void on_actionReboot_triggered();



    void deleteRecord(QString descrip);

    bool isConnectedToNetwork();



    void on_actionRecovery_triggered();



    void on_donate_clicked();

   void do_versioncheck();




    void on_actionDownload_Kodi_triggered();

    void on_editRecord_clicked();

    void dataentry(bool isNewRecord);
    //void newentry(bool isNewRecord);


  //   void move_kodi_data(QString externallocation, int choice);




    void on_doConsole_clicked();

    void on_killServer_clicked();


    void on_newRecord_clicked();

   void  screenCap();


   void on_delRecord_clicked();


   void on_cacheButton_clicked();

 //  void on_splashButton_clicked();

   void on_mvdataButton_clicked();

   void on_actionView_Kodi_Log_triggered();

   void on_actionView_adbLink_Log_triggered();


   void on_stopapp_clicked();




  // void on_actionEdit_XML_files_triggered();


   void on_actionPaste_path_triggered();

   void on_actionStop_Application_triggered();

   void on_actionStart_Application_triggered();

   void on_keypadButton_clicked();



   void on_fmButton_clicked();



   void on_actionMount_system_RO_triggered();

   void on_actionMount_system_RW_triggered();

   void on_actionWireless_ADBD_triggered();

   void on_actionKodi_data_usage_triggered();


  void on_clearButton_clicked();

  void on_actionReboot_bootloader_triggered();

  void on_actionPreferences_triggered();

  void updateckversion(int value);



   bool xcheck_devices();



  void on_adhocip_returnPressed();

  void on_actionReiinstall_Busybox_triggered();

  void on_actionArchitecture_triggered();

  void on_actionSleep_adjust_triggered();


  void on_actiondelthumb_triggered();

  void on_actionTest_adb_connection_triggered();

  void on_actionPush_remote_triggered();


  void on_actionCreate_kodi_data_triggered();



  void on_scpyButton_clicked();



  void on_pushButton_clicked();

  void on_pushTimers_clicked();




  void on_actionView_Changelog_triggered();

  void  on_startapp_clicked();

  void on_actionSplash_Screen_triggered();

  void on_View_Changelog_triggered();

  void on_Erase_adbLink_database_triggered();

 void on_actionSet_Kodi_permissions_triggered();

  void on_actionGet_UID_from_APK_file_triggered();

  void on_actionOculus_VR_triggered();

  void on_actionSend_text_triggered();


  void on_test_clicked();

  void on_actionEdit_XML_triggered();

  void on_actionScreen_Capture_triggered();

  void on_actionKeypad_triggered();

  void on_ascend_clicked();

  void on_descend_clicked();



  void on_actionSwitch_View_triggered();


  void on_screencap1_clicked();

  void on_screencap2_clicked();

  void on_infoButton_clicked();

  private:
    Ui::MainWindow *ui;

 usbfileDialog *fmdialog;

 QStringList RunProcessList;



};




#endif // MAINWINDOW_H
