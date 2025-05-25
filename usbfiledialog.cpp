#include "usbfiledialog.h"
#include "ui_usbfiledialog.h"
#include "customlistwidget.h"
#include "editordialog.h"
#include "dragdialog.h"
#include "logfile.h"
#include "getadbdata.h"
#include <QProcess>
#include <QFile>
#include <QString>
#include <QMessageBox>
#include <QStringList>
#include <QTextStream>
#include <QInputDialog>
#include <QTreeView>
#include <QFileDialog>
#include <QFileInfo>
#include <QDirModel>
#include <QDebug>
#include <QTimer>
#include <QStatusBar>
#include <QFileSystemModel>
#include <QApplication>
#include <QDir>

#ifdef Q_OS_LINUX
 int ost1=0;
#elif defined(Q_OS_WIN)
  int ost1=1;
#elif defined(Q_OS_MAC)
int ost1=2;
#endif

bool hasfocus=false;
QString ufdlogfiledir="";
QString quote1="\"'";
QString quote2="'\"";
bool do_oldfm=false;

static const QString adbShell = " shell ";

usbfileDialog::usbfileDialog(bool iskodi, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::usbfileDialog)
{
    ui->setupUi(this);

    // Initialize directories if empty
    if (current_directory1.isEmpty()) current_directory1 = "/sdcard/";
    if (current_directory2.isEmpty()) current_directory2 = "/sdcard/";

    // Set currentDirectory property for drag-and-drop
    ui->usblistWidget1->setProperty("currentDirectory", current_directory1);
    ui->usblistWidget2->setProperty("currentDirectory", current_directory2);

    // Ensure drag-and-drop properties
    ui->usblistWidget1->setDragEnabled(true);
    ui->usblistWidget1->setAcceptDrops(true);
    ui->usblistWidget1->setDragDropMode(QAbstractItemView::DragDrop);
    ui->usblistWidget2->setDragEnabled(true);
    ui->usblistWidget2->setAcceptDrops(true);
    ui->usblistWidget2->setDragDropMode(QAbstractItemView::DragDrop);

    if (ost1 == 1)
    {
        ufdlogfiledir = QDir::homePath() + "/AppData/Roaming/.jocala/";
        tmpdir1 = QDir::homePath() + "/AppData/Roaming/.jocala/scripts/";
    }
    else
    {
        ufdlogfiledir = QDir::homePath() + "/.jocala/";
        tmpdir1 = QDir::homePath() + "/.jocala/scripts/";
    }

    ui->kodiDirs->clear();
    ui->kodiDirs->addItem("sdcard");
    ui->kodiDirs->addItem("storage");
    ui->kodiDirs->addItem("data/local/tmp");

    if (iskodi)
    {
        ui->kodiDirs->addItem("kodi");
        ui->kodiDirs->addItem("userdata");
        ui->kodiDirs->addItem("addons");
    }

    ui->usbprogressBar->setHidden(true);
    ui->usbprogressBar->setValue(0);

    // Connect existing signals
    connect(ui->usblistWidget1, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(assignWindow1()));
    connect(ui->usblistWidget2, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(assignWindow2()));

    // Connect filesDropped signals for internal drag-and-drop
    connect(static_cast<CustomListWidget *>(ui->usblistWidget1), &CustomListWidget::filesDropped, this, &usbfileDialog::handleFilesDropped);
    connect(static_cast<CustomListWidget *>(ui->usblistWidget2), &CustomListWidget::filesDropped, this, &usbfileDialog::handleFilesDropped);

    // Connect externalFilesDropped signals for desktop file drops
    connect(static_cast<CustomListWidget *>(ui->usblistWidget1), &CustomListWidget::externalFilesDropped, this, &usbfileDialog::onExternalFilesDropped);
    connect(static_cast<CustomListWidget *>(ui->usblistWidget2), &CustomListWidget::externalFilesDropped, this, &usbfileDialog::onExternalFilesDropped);

    // Connect focusRequested signals for context menu
    connect(static_cast<CustomListWidget *>(ui->usblistWidget1), &CustomListWidget::focusRequested, this, &usbfileDialog::assignWindow1);
    connect(static_cast<CustomListWidget *>(ui->usblistWidget2), &CustomListWidget::focusRequested, this, &usbfileDialog::assignWindow2);
}

usbfileDialog::~usbfileDialog()
{
    delete ui;
}

void usbfileDialog::setMode(const int &modenum)
{
    if (modenum == 0)
        ui->usblistWidget1->setSelectionMode(QAbstractItemView::ExtendedSelection);
    else
        ui->usblistWidget1->setSelectionMode(QAbstractItemView::SingleSelection);
}

void usbfileDialog::setData(const QString &labelText)
{
    this->setWindowTitle(labelText);
}

void usbfileDialog::setPulldir(const QString &pullstr)
{
    pulldir_ufd = pullstr;
}

void usbfileDialog::setuProgram(const QString &fprogram)
{
    fmprogram = fprogram;
}

void usbfileDialog::setAdbdir(const QString &adbstr)
{
    adbdir_ufd = adbstr;
}

void usbfileDialog::setADB(const QString &adbdata)
{
    adb21 = adbdata;
}

void usbfileDialog::setkodiPath(const QString &kodipath)
{
    // Use kodipath as a non-root path, default to /sdcard/ if invalid
    if (!is_directory(kodipath))
        current_directory1 = "/sdcard/";
    else
        current_directory1 = kodipath;
}

void usbfileDialog::on_pushfilesButton_clicked()
{
    QStringList filePaths;
    QString xpath;

    if (hasfocus)
        xpath = current_directory1;
    else
        xpath = current_directory2;

    dragDialog dialog;
    dialog.setWindowTitle(xpath);

    if (dialog.exec() == QDialog::Accepted)
        filePaths = dialog.getFilePaths();

    if (filePaths.isEmpty())
        return;

    userpush(filePaths);
}

void usbfileDialog::on_pullButton_clicked()
{
    if (!QDir(pulldir_ufd).exists())
    {
        QMessageBox::critical(this, "", "Pull destination does not exist");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "", "Pull file(s)?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No)
        return;

    QStringList mstringlist;
    QString fileName;
    QString fname;
    QString command;
    QString cstring;
    bool dtest;
    QString dirname;
    int error = 0;
    bool overwriteRejected = false;

    if (hasfocus)
    {
        if (ui->usblistWidget1->selectedItems().count() >= 1)
        {
            foreach (QListWidgetItem *item, ui->usblistWidget1->selectedItems())
            {
                if (item->text() == "..")
                    return;
                else
                    mstringlist << item->text();
            }
        }
    }
    else
    {
        if (ui->usblistWidget2->selectedItems().count() >= 1)
        {
            foreach (QListWidgetItem *item, ui->usblistWidget2->selectedItems())
            {
                if (item->text() == "..")
                    return;
                else
                    mstringlist << item->text();
            }
        }
    }

    if (mstringlist.isEmpty())
        return;

    logfile("Pull Files");
    logfile("----------");

    for (QStringList::iterator it = mstringlist.begin(); it != mstringlist.end(); ++it)
    {
        fileName = *it;
        int r = fileName.lastIndexOf("/");
        fname = fileName.mid(r + 1, fileName.length());

        QString destFilePath = pulldir_ufd + "/" + fname;

        if (QFile::exists(destFilePath))
        {
            QMessageBox::StandardButton overwriteReply;
            overwriteReply = QMessageBox::question(this, "", QString("File %1 already exists. Overwrite?").arg(fname),
                                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            if (overwriteReply == QMessageBox::Cancel)
                return;
            else if (overwriteReply == QMessageBox::No)
            {
                logfile(destFilePath + " not overwritten.");
                overwriteRejected = true;
                continue;
            }
        }

        cstring = adb21 + adbShell + " test -d " + fileName + " && echo true || echo false";
        command = RunLongProcess_ufd(cstring);

        if (command.contains("true"))
        {
            int r = fileName.lastIndexOf("/");
            dirname = fileName.mid(r, fileName.count());
            dirname = fix_directory(dirname);
            dtest = true;
        }
        else
        {
            dtest = false;
        }

        if (dtest)
            cstring = adb21 + " pull " + '"' + fileName + '"' + " " + '"' + pulldir_ufd + dirname + '"';
        else
            cstring = adb21 + " pull " + '"' + fileName + '"' + " " + '"' + pulldir_ufd + '"';

        command = RunLongProcess_ufd(cstring);

        if (!command.contains("pulled"))
        {
            cstring = adb21 + adbShell + " cp -R " + fileName + " /data/local/tmp";
            command = RunLongProcess_ufd(cstring);

            if (!command.isEmpty())
            {
                error++;
                logfile("Error: " + command);
                QMessageBox::critical(this, "", "Error(s). See log");
                return;
            }

            fname = "/data/local/tmp/" + fname;
            cstring = adb21 + adbShell + " chmod -R 755 " + fname;
            command = getadbOutput(cstring);

            if (dtest)
                cstring = adb21 + " pull " + '"' + fname + '"' + " " + '"' + pulldir_ufd + dirname + '"';
            else
                cstring = adb21 + " pull " + '"' + fname + '"' + " " + '"' + pulldir_ufd + '"';

            QString cmd = RunLongProcess_ufd(cstring);

            if (!cmd.contains("pulled"))
            {
                logfile("pull failed:" + fileName);
                logfile(cstring);
                logfile(cmd);
                error++;
            }

            cstring = adb21 + adbShell + " rm -r " + fname;
            command = getadbOutput(cstring);
        }
        else
        {
            logfile("Destination: " + pulldir_ufd + "\n" + fileName);
        }
    }

    if (error > 0)
    {
        QMessageBox::critical(this, "", "Error(s). See log");
    }
    else if (overwriteRejected)
    {
        QMessageBox::information(this, "", "Pull(s) complete. Some files were not overwritten. See log.");
    }
    else
    {
        QMessageBox::information(this, "", "Pull(s) complete. See log.");
    }

    ui->usblistWidget1->clearSelection();
    ui->usblistWidget2->clearSelection();

    setPath1(current_directory1);
    setPath2(current_directory2);
}

void usbfileDialog::on_delButton_clicked()
{
    QStringList mstringlist;
    QString fileName;
    QString command;
    QString cstring;
    QString workingdir;
    int error = 0;

    if (ui->usblistWidget1->selectedItems().count() >= 1 && ui->usblistWidget2->selectedItems().count() >= 1)
    {
        QMessageBox::critical(this, "", "Select from one pane only");
        return;
    }

    if (hasfocus)
    {
        if (ui->usblistWidget1->selectedItems().count() >= 1)
        {
            foreach (QListWidgetItem *item, ui->usblistWidget1->selectedItems())
            {
                if (item->text() == "..")
                    return;
                else
                    mstringlist << item->text();
            }
            current_directory1 = mstringlist.at(0).left(mstringlist.at(0).lastIndexOf("/"));
            if (!current_directory1.endsWith("/"))
                current_directory1.append("/");
            workingdir = current_directory1;
        }
    }
    else
    {
        if (ui->usblistWidget2->selectedItems().count() >= 1)
        {
            foreach (QListWidgetItem *item, ui->usblistWidget2->selectedItems())
            {
                if (item->text() == "..")
                    return;
                else
                    mstringlist << item->text();
            }
            current_directory2 = mstringlist.at(0).left(mstringlist.at(0).lastIndexOf("/"));
            if (!current_directory2.endsWith("/"))
                current_directory2.append("/");
            workingdir = current_directory2;
        }
    }

    QMessageBox::StandardButton reply3;
    reply3 = QMessageBox::question(this, "", "Delete file(s)?", QMessageBox::Yes | QMessageBox::No);
    if (reply3 == QMessageBox::No)
        return;

    logfile("Delete Files");
    logfile("----------");

    for (QStringList::iterator it = mstringlist.begin(); it != mstringlist.end(); ++it)
    {
        fileName = *it;
        cstring = adb21 + adbShell + " rm -r " + quote1 + fileName + quote2;
        command = RunLongProcess_ufd(cstring);

        if (command.contains("exist"))
        {
            logfile(command);
            logfile("Deletion failed");
            QMessageBox::critical(this, "", fileName + ": deletion failed");
            error++;
        }
        else
        {
            logfile(fileName + ": deletion succeeded");
        }
    }

    setPath1(current_directory1);
    setPath2(current_directory2);

    if (error > 0)
        QMessageBox::critical(this, "", "Delete(s) failed. See log.");
}

void usbfileDialog::on_editButton_clicked()
{
    QStringList mstringlist;
    QString curdir;
    QString fileName;

    if (hasfocus)
    {
        if (ui->usblistWidget1->selectedItems().count() == 1)
        {
            foreach (QListWidgetItem *item, ui->usblistWidget1->selectedItems())
            {
                if (item->text() == "..")
                    return;
                else
                    mstringlist << item->text();
            }
            fileName = mstringlist.at(0);
            curdir = current_directory1;
        }
    }
    else
    {
        if (ui->usblistWidget2->selectedItems().count() == 1)
        {
            foreach (QListWidgetItem *item, ui->usblistWidget2->selectedItems())
            {
                if (item->text() == "..")
                    return;
                else
                    mstringlist << item->text();
            }
            fileName = mstringlist.at(0);
            curdir = current_directory2;
        }
    }

    if (fileName.isEmpty())
        return;

    editfile(fileName, curdir);
    setPath1(curdir);
}

void usbfileDialog::on_resetButton_clicked()
{
    setPath1("/sdcard/");
    setPath2("/sdcard/");
    ui->customdir->setText("");
}

void usbfileDialog::on_goButton_clicked()
{
    QString xpath;
    QString dirpath = ui->customdir->text();

    if (!dirpath.isEmpty())
        xpath = dirpath;
    else
    {
        switch (ui->kodiDirs->currentIndex())
        {
        case 0:
            xpath = "/sdcard/";
            break;
        case 1:
            xpath = "/storage/";
            break;
        case 2:
            xpath = "/data/local/tmp/";
            break;
        case 3:
            xpath = "/sdcard/kodi/"; // Default non-root Kodi path
            break;
        case 4:
            xpath = "/sdcard/kodi/userdata/";
            break;
        case 5:
            xpath = "/sdcard/kodi/addons/";
            break;
        default:
            xpath = "/sdcard/";
            break;
        }
    }

    if (!is_directory(xpath))
    {
        QMessageBox::critical(this, "", xpath + " :Directory not found.");
        return;
    }

    if (ui->usblistWidget2->hasFocus())
        setPath2(xpath);
    else
        setPath1(xpath);
}

void usbfileDialog::on_mkdirButton_clicked()
{
    bool ok;
    QString xpath;
    QString newdir = QInputDialog::getText(this, "", "New Directory", QLineEdit::Normal, "", &ok);

    if (hasfocus)
        xpath = current_directory1;
    else
        xpath = current_directory2;

    if (newdir.isEmpty())
        return;

    newdir = xpath + newdir;
    QString cstring = adb21 + adbShell + " mkdir -p " + quote1 + newdir + quote2;
    QString command = getadbOutput(cstring);

    setPath1(current_directory1);
    setPath2(current_directory2);
}

void usbfileDialog::on_renameButton_clicked()
{
    QStringList mstringlist;
    int selected_items = 0;
    bool ok;
    QString xpath;
    QString oldname;

    if (hasfocus)
    {
        xpath = current_directory1;
        selected_items = ui->usblistWidget1->selectedItems().count();
        foreach (QListWidgetItem *item, ui->usblistWidget1->selectedItems())
        {
            if (item->text() == "..")
                return;
            else
                mstringlist << item->text();
        }
    }
    else
    {
        xpath = current_directory2;
        selected_items = ui->usblistWidget2->selectedItems().count();
        foreach (QListWidgetItem *item, ui->usblistWidget2->selectedItems())
        {
            if (item->text() == "..")
                return;
            else
                mstringlist << item->text();
        }
    }

    if (selected_items != 1)
    {
        QMessageBox::critical(this, "", "Must select 1 item only");
        return;
    }

    QString newname = QInputDialog::getText(this, "", "New name", QLineEdit::Normal, "", &ok);
    if (newname.isEmpty())
        return;

    oldname = mstringlist.at(0);
    newname = xpath + newname;

    QString cstring = adb21 + adbShell + " mv " + quote1 + oldname + quote2 + " " + quote1 + newname + quote2;
    QString command = getadbOutput(cstring);
    logfile(cstring);
    logfile(command);

    setPath1(current_directory1);
    setPath2(current_directory2);
}

void usbfileDialog::on_customdir_returnPressed()
{
    on_goButton_clicked();
}

QString usbfileDialog::fix_directory(QString dirname)
{
    if (!dirname.startsWith("/"))
        dirname.prepend("/");
    if (!dirname.endsWith("/"))
        dirname.append("/");
    return dirname;
}

void usbfileDialog::userpush(QStringList filenames)
{
    QString command;
    QString cstring;
    QString xpath;

    logfile("userpush started");

    if (filenames.isEmpty())
        return;

    bool error = false;

    if (hasfocus)
        xpath = current_directory1;
    else
        xpath = current_directory2;

    for (int i = 0; i < filenames.count(); i++)
    {
        QFileInfo info1(filenames.at(i));
        QString fname = info1.fileName();
        QString absolutefname = info1.absoluteFilePath();

        QString tmpstr = xpath + fname;
        tmpstr = tmpstr.trimmed();
        tmpstr = "\"" + tmpstr + "\"";
        absolutefname = "\"" + absolutefname + "\"";

        cstring = adb21 + " push " + absolutefname + " " + tmpstr;
        command = RunLongProcess_ufd(cstring);
        logfile(cstring);
        logfile(command);

        if (!command.contains("bytes"))
        {
            error = true;
            logfile(info1.fileName() + " not pushed");
            logfile(command);
        }
        else
        {
            logfile(info1.fileName() + " pushed");
        }
    }

    setPath1(current_directory1);
    setPath2(current_directory2);

    if (error)
        QMessageBox::critical(this, "", "File(s) not pushed. See log.");
}

QString usbfileDialog::RunLongProcess_ufd(QString cstring)
{
    ui->usbprogressBar->setHidden(false);
    ui->usbprogressBar->setValue(0);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(usbTimerEvent()));
    timer->start(2000);

    QString command = getadbOutput(cstring);

    ui->usbprogressBar->setHidden(true);
    ui->usbprogressBar->setValue(0);
    timer->stop();
    delete timer;
    return command;
}

void usbfileDialog::on_usblistWidget2_doubleClicked(const QModelIndex &index)
{
    currentitem2 = index.data(Qt::DisplayRole).toString();

    if (currentitem2.contains(".."))
    {
        previous_directory2 = previous_directory2.left(previous_directory2.lastIndexOf("/"));
        previous_directory2 = previous_directory2.left(previous_directory2.lastIndexOf("/"));
        previous_directory2 = fix_directory(previous_directory2);
        setPath2(previous_directory2);
        return;
    }

    QString cstring = adb21 + adbShell + " [ -d " + quote1 + currentitem2 + quote2 + " ] && echo 'true'";
    QString command = getadbOutput(cstring);

    if (command.contains("true"))
    {
        currentitem2 = fix_directory(currentitem2);
        setPath2(currentitem2);
    }
    else
    {
        on_pullButton_clicked();
    }
}

void usbfileDialog::on_usblistWidget1_doubleClicked(const QModelIndex &index)
{
    currentitem1 = index.data(Qt::DisplayRole).toString();

    if (currentitem1.contains(".."))
    {
        previous_directory1 = previous_directory1.left(previous_directory1.lastIndexOf("/"));
        previous_directory1 = previous_directory1.left(previous_directory1.lastIndexOf("/"));
        previous_directory1 = fix_directory(previous_directory1);
        setPath1(previous_directory1);
        return;
    }

    QString cstring = adb21 + adbShell + " [ -d " + quote1 + currentitem1 + quote2 + " ] && echo 'true'";
    QString command = getadbOutput(cstring);

    if (command.contains("true"))
    {
        currentitem1 = fix_directory(currentitem1);
        setPath1(currentitem1);
    }
    else
    {
        on_pullButton_clicked();
    }
}

bool usbfileDialog::is_directory(QString fdirectory)
{
    QString string1 = adb21 + adbShell + "/data/local/tmp/adblink/busybox find " + '"' + fdirectory + '"' + " -type d -maxdepth 0";
    QString command = getadbOutput(string1);

    if (command.contains("No such file or directory"))
        return false;
    else
        return true;
}

void usbfileDialog::setPath1(QString currentdir)
{
    qDebug() << "setPath1 called with:" << currentdir;

    current_directory1 = currentdir;
    previous_directory1 = currentdir.left(currentdir.lastIndexOf("/"));

    if (previous_directory1.isEmpty())
        previous_directory1 = current_directory1;

    current_directory1 = fix_directory(current_directory1);
    previous_directory1 = fix_directory(previous_directory1);

    ui->usblistWidget1->setProperty("currentDirectory", current_directory1);
    qDebug() << "usblistWidget1 currentDirectory set to:" << ui->usblistWidget1->property("currentDirectory").toString();

    currentdir.replace(" ", "\\ ");
    currentdir.replace("'", "\\'");

    QString string1 = adb21 + adbShell + "/data/local/tmp/adblink/busybox find " + '"' + currentdir + '"' + " -type d -maxdepth 1";
    QString command = getadbOutput(string1);
    QStringList stringlist1 = command.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

    stringlist1.sort();

    QString string2 = adb21 + adbShell + "/data/local/tmp/adblink/busybox find " + '"' + currentdir + '"' + " -type l -maxdepth 1";
    command = getadbOutput(string2);
    QStringList stringlist2 = command.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

    stringlist2.sort();

    QString string3 = adb21 + adbShell + "/data/local/tmp/adblink/busybox find " + '"' + currentdir + '"' + " -type f -maxdepth 1";
    command = getadbOutput(string3);
    QStringList stringlist3 = command.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

    stringlist3.sort();

    QStringList stringlist4 = stringlist1 + stringlist2 + stringlist3;

    stringlist4.removeFirst();

    ui->usblistWidget1->clear();

    for (QStringList::const_iterator it = stringlist4.begin(); it != stringlist4.end(); ++it)
    {
        currentitem1 = *it;
        if (!currentitem1.contains("Permission")
            && !currentitem1.contains("denied")
            && !currentitem1.contains("emulated")
            && !currentitem1.contains(":")
            && !currentitem1.isEmpty())
        {
            QListWidgetItem *item = new QListWidgetItem(currentitem1);
            item->setFlags(item->flags() | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            ui->usblistWidget1->addItem(item);
            qDebug() << "Added item to usblistWidget1:" << currentitem1 << "flags:" << item->flags()
                     << "dragEnabled:" << (item->flags() & Qt::ItemIsDragEnabled);
        }
    }

    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText("..");
    newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->usblistWidget1->insertItem(0, newItem);
    qDebug() << "Added .. item to usblistWidget1, flags:" << newItem->flags()
             << "dragEnabled:" << (newItem->flags() & Qt::ItemIsDragEnabled);

    qDebug() << "usblistWidget1 item count after setPath1:" << ui->usblistWidget1->count();
}

void usbfileDialog::setPath2(QString currentdir)
{
    qDebug() << "setPath2 called with:" << currentdir;

    current_directory2 = currentdir;
    previous_directory2 = currentdir.left(currentdir.lastIndexOf("/"));

    if (previous_directory2.isEmpty())
        previous_directory2 = current_directory2;

    current_directory2 = fix_directory(current_directory2);
    previous_directory2 = fix_directory(previous_directory2);

    ui->usblistWidget2->setProperty("currentDirectory", current_directory2);
    qDebug() << "usblistWidget2 currentDirectory set to:" << ui->usblistWidget2->property("currentDirectory").toString();

    currentdir.replace(" ", "\\ ");
    currentdir.replace("'", "\\'");

    QString string1 = adb21 + adbShell + "/data/local/tmp/adblink/busybox find " + '"' + currentdir + '"' + " -type d -maxdepth 1";
    QString command = getadbOutput(string1);
    QStringList stringlist1 = command.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

    stringlist1.sort();

    QString string2 = adb21 + adbShell + "/data/local/tmp/adblink/busybox find " + '"' + currentdir + '"' + " -type l -maxdepth 1";
    command = getadbOutput(string2);
    QStringList stringlist2 = command.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

    stringlist2.sort();

    QString string3 = adb21 + adbShell + "/data/local/tmp/adblink/busybox find " + '"' + currentdir + '"' + " -type f -maxdepth 1";
    command = getadbOutput(string3);
    QStringList stringlist3 = command.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

    stringlist3.sort();

    QStringList stringlist4 = stringlist1 + stringlist2 + stringlist3;

    stringlist4.removeFirst();

    ui->usblistWidget2->clear();

    for (QStringList::const_iterator it = stringlist4.begin(); it != stringlist4.end(); ++it)
    {
        currentitem2 = *it;
        if (!currentitem2.contains("Permission")
            && !currentitem2.contains("denied")
            && !currentitem2.contains("emulated")
            && !currentitem2.contains(":")
            && !currentitem2.isEmpty())
        {
            QListWidgetItem *item = new QListWidgetItem(currentitem2);
            item->setFlags(item->flags() | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            ui->usblistWidget2->addItem(item);
            qDebug() << "Added item to usblistWidget2:" << currentitem2 << "flags:" << item->flags()
                     << "dragEnabled:" << (item->flags() & Qt::ItemIsDragEnabled);
        }
    }

    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText("..");
    newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->usblistWidget2->insertItem(0, newItem);
    qDebug() << "Added .. item to usblistWidget2, flags:" << newItem->flags()
             << "dragEnabled:" << (newItem->flags() & Qt::ItemIsDragEnabled);

    qDebug() << "usblistWidget2 item count after setPath2:" << ui->usblistWidget2->count();
}

void usbfileDialog::editfile(QString fileName, QString xpath)
{
    QString tempfile1;
    QString tempfile2;
    QString backfile;
    QString cstring;
    QString command;

    QString tmpdir = "/data/local/tmp/";
    QString filename = fileName.mid(fileName.lastIndexOf("/") + 1);

    cstring = adb21 + adbShell + " test -d " + quote1 + fileName + quote2 + " && echo 'true' || echo 'false'";
    command = getadbOutput(cstring);

    if (command.contains("true"))
    {
        QMessageBox::critical(this, "", "Can't edit directory " + fileName);
        return;
    }

    cstring = adb21 + " pull " + quote1 + fileName + quote2 + " " + quote1 + tmpdir1 + "/" + filename + quote2;
    command = getadbOutput(cstring);

    if (!command.contains("bytes"))
    {
        logfile("edit failed");
        logfile(command);
        QMessageBox::critical(this, "", "Edit failed: " + command);
        return;
    }

    QFile file1(tmpdir1 + filename);
    if (!file1.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString xmlfile = file1.readAll();
    editorDialog dialog;
    dialog.seteditor(xmlfile);
    dialog.setfilename(filename);

    backfile = filename + ".bak";
    tempfile1 = tmpdir1 + filename;
    tempfile2 = tmpdir1 + filename + ".bak";

    dialog.setModal(true);

    if (dialog.exec() == QDialog::Accepted)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Save", "Save " + fileName + "?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No)
            return;

        xmlfile = dialog.xmlfile();

        QFile::copy(tempfile1, tempfile2);
        QFile caFile(tempfile1);
        caFile.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream outStream(&caFile);
        outStream << xmlfile;
        caFile.close();

        cstring = adb21 + " push " + quote1 + tempfile1 + quote2 + " " + quote1 + xpath + quote2;
        command = getadbOutput(cstring);

        cstring = adb21 + " push " + quote1 + tempfile2 + quote2 + " " + quote1 + xpath + quote2;
        command = getadbOutput(cstring);

        file1.close();
        QFile file2(tempfile1);
        file2.remove();
        QFile file3(tempfile2);
        file3.remove();
    }
}

void usbfileDialog::assignWindow1()
{
    hasfocus = true;
}

void usbfileDialog::assignWindow2()
{
    hasfocus = false;
}

void usbfileDialog::usbTimerEvent()
{
    int value = ui->usbprogressBar->value();

    if (value >= 100)
    {
        value = 0;
        ui->usbprogressBar->reset();
    }

    ui->usbprogressBar->setValue(value + 1);
}

void usbfileDialog::gather_push()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Files"), QDir::currentPath(), tr("All files (*.*)"));

    if (!filenames.isEmpty())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Push", "Push files?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            userpush(filenames);
        }
    }
}

void usbfileDialog::on_copyButton_clicked()
{
    do_copy(0);
}

void usbfileDialog::on_moveButton_clicked()
{
    do_copy(1);
}

void usbfileDialog::do_copy(int opcode)
{
    QStringList mstringlist;
    QString fileName;
    QString command;
    QString cstring;
    QString workingdir;
    QString optext;
    int error = 0;

    if (opcode == 0)
        optext = "Copy";
    else
        optext = "Move";

    if (hasfocus)
    {
        if (ui->usblistWidget1->selectedItems().count() >= 1)
        {
            foreach (QListWidgetItem *item, ui->usblistWidget1->selectedItems())
            {
                if (item->text() == "..")
                    return;
                else
                    mstringlist << item->text();
            }
            current_directory1 = mstringlist.at(0).left(mstringlist.at(0).lastIndexOf("/"));
            if (!current_directory1.endsWith("/"))
                current_directory1.append("/");
            workingdir = current_directory2;
        }
    }
    else
    {
        if (ui->usblistWidget2->selectedItems().count() >= 1)
        {
            foreach (QListWidgetItem *item, ui->usblistWidget2->selectedItems())
            {
                if (item->text() == "..")
                    return;
                else
                    mstringlist << item->text();
            }
            current_directory2 = mstringlist.at(0).left(mstringlist.at(0).lastIndexOf("/"));
            if (!current_directory2.endsWith("/"))
                current_directory2.append("/");
            workingdir = current_directory1;
        }
    }

    if (mstringlist.isEmpty())
        return;

    QMessageBox::StandardButton reply3;
    reply3 = QMessageBox::question(this, "", optext + " item(s)?", QMessageBox::Yes | QMessageBox::No);
    if (reply3 == QMessageBox::No)
        return;

    logfile(optext + " files");
    logfile("----------");

    for (QStringList::iterator it = mstringlist.begin(); it != mstringlist.end(); ++it)
    {
        fileName = *it;
        if (opcode == 0)
            cstring = adb21 + adbShell + " cp -R " + quote1 + fileName + quote2 + " " + quote1 + workingdir + quote2;
        else
            cstring = adb21 + adbShell + " mv " + quote1 + fileName + quote2 + " " + quote1 + workingdir + quote2;

        logfile(cstring);
        command = RunLongProcess_ufd(cstring);

        if (!command.isEmpty())
        {
            logfile(command);
            logfile(optext + " failed");
            QMessageBox::critical(this, "", fileName + " " + optext + " failed");
            error++;
        }
        else
        {
            logfile(fileName + " " + optext + " succeeded");
        }
    }

    if (error > 0)
        QMessageBox::critical(this, "", optext + "(s) failed. See log.");

    setPath1(current_directory1);
    setPath2(current_directory2);
}

void usbfileDialog::do_xcopy(int /*opcode*/)
{
    qDebug() << "do_xcopy function entered";

    QStringList mstringlist;
    QString fileName;
    QString command;
    QString cstring;
    QString workingdir;
    QString optext;
    int error = 0;
    int opcode = 0;

    if (hasfocus)
    {
        qDebug() << "Source: usblistWidget2, Target: usblistWidget1";
        if (ui->usblistWidget2->selectedItems().count() >= 1)
        {
            foreach (QListWidgetItem *item, ui->usblistWidget2->selectedItems())
            {
                if (item->text() == "..")
                {
                    qDebug() << "Skipping .. item";
                    continue;
                }
                mstringlist << item->text();
            }
            workingdir = current_directory1;
        }
    }
    else
    {
        qDebug() << "Source: usblistWidget1, Target: usblistWidget2";
        if (ui->usblistWidget1->selectedItems().count() >= 1)
        {
            foreach (QListWidgetItem *item, ui->usblistWidget1->selectedItems())
            {
                if (item->text() == "..")
                {
                    qDebug() << "Skipping .. item";
                    continue;
                }
                mstringlist << item->text();
            }
            workingdir = current_directory2;
        }
    }

    qDebug() << "mstringlist:" << mstringlist;
    if (mstringlist.isEmpty())
    {
        qDebug() << "No valid files selected, exiting do_xcopy";
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("");
    msgBox.setText("Choose operation for " + QString::number(mstringlist.count()) + " item(s):");
    QAbstractButton *moveButton = msgBox.addButton("Move", QMessageBox::ActionRole);
    QAbstractButton *copyButton = msgBox.addButton("Copy", QMessageBox::ActionRole);
    QAbstractButton *cancelButton = msgBox.addButton("Cancel", QMessageBox::RejectRole);
    msgBox.exec();

    if (msgBox.clickedButton() == cancelButton)
    {
        qDebug() << "User cancelled operation";
        return;
    }
    else if (msgBox.clickedButton() == copyButton)
    {
        opcode = 0;
        optext = "Copy";
    }
    else if (msgBox.clickedButton() == moveButton)
    {
        opcode = 1;
        optext = "Move";
    }

    qDebug() << "Selected operation:" << optext << "opcode:" << opcode;

    logfile(optext + " files");
    logfile("----------");

    for (QStringList::iterator it = mstringlist.begin(); it != mstringlist.end(); ++it)
    {
        fileName = *it;
        qDebug() << "Processing file:" << fileName;

        if (opcode == 0)
            cstring = adb21 + adbShell + " cp -R " + quote1 + fileName + quote2 + " " + quote1 + workingdir + quote2;
        else
            cstring = adb21 + adbShell + " mv " + quote1 + fileName + quote2 + " " + quote1 + workingdir + quote2;

        qDebug() << "adb command:" << cstring;
        logfile(cstring);

        command = RunLongProcess_ufd(cstring);

        if (!command.isEmpty())
        {
            qDebug() << "adb result:" << command << "error: " + optext + " failed";
            logfile(command);
            logfile(optext + " failed");
            QMessageBox::critical(this, "", fileName + " " + optext + " failed");
            error++;
        }
        else
        {
            qDebug() << "adb result: success";
            logfile(fileName + " " + optext + " succeeded");
        }
    }

    if (error > 0)
    {
        qDebug() << optext + " errors occurred";
        QMessageBox::critical(this, "", optext + "(s) failed. See log.");
    }

    qDebug() << "Refreshing widgets with current_directory1:" << current_directory1 << "current_directory2:" << current_directory2;
    setPath1(current_directory1);
    setPath2(current_directory2);

    qDebug() << "do_xcopy completed";
}

void usbfileDialog::handleFilesDropped(const QStringList &fileNames, const QString &targetDir)
{
    qDebug() << "handleFilesDropped called with fileNames:" << fileNames << "targetDir:" << targetDir;
    bool droppedOnWidget1 = (targetDir == current_directory1);
    hasfocus = droppedOnWidget1;

    QStringList mstringlist;
    for (const QString &fileName : fileNames)
    {
        if (fileName != "..")
        {
            mstringlist << fileName;
        }
    }
    qDebug() << "mstringlist:" << mstringlist;

    if (mstringlist.isEmpty())
    {
        qDebug() << "No valid files to copy/move";
        return;
    }

    if (droppedOnWidget1)
    {
        current_directory1 = targetDir;
        current_directory2 = mstringlist.first().left(mstringlist.first().lastIndexOf("/"));
        if (!current_directory2.endsWith("/"))
            current_directory2.append("/");
        ui->usblistWidget2->clearSelection();
        for (const QString &fileName : mstringlist)
        {
            for (int i = 0; i < ui->usblistWidget2->count(); ++i)
            {
                if (ui->usblistWidget2->item(i)->text() == fileName)
                {
                    ui->usblistWidget2->item(i)->setSelected(true);
                    qDebug() << "Selected item in usblistWidget2:" << fileName;
                    break;
                }
            }
        }
    }
    else
    {
        current_directory2 = targetDir;
        current_directory1 = mstringlist.first().left(mstringlist.first().lastIndexOf("/"));
        if (!current_directory1.endsWith("/"))
            current_directory1.append("/");
        ui->usblistWidget1->clearSelection();
        for (const QString &fileName : mstringlist)
        {
            for (int i = 0; i < ui->usblistWidget1->count(); ++i)
            {
                if (ui->usblistWidget1->item(i)->text() == fileName)
                {
                    ui->usblistWidget1->item(i)->setSelected(true);
                    qDebug() << "Selected item in usblistWidget1:" << fileName;
                    break;
                }
            }
        }
    }

    qDebug() << "Updated directories: current_directory1:" << current_directory1 << "current_directory2:" << current_directory2;
    qDebug() << "hasfocus:" << hasfocus;
    qDebug() << "Calling do_xcopy";
    do_xcopy(0);
    qDebug() << "do_xcopy completed in handleFilesDropped";
}

void usbfileDialog::onExternalFilesDropped(const QStringList &filePaths, const QString &targetDir)
{
    if (sender() == ui->usblistWidget1)
    {
        current_directory1 = targetDir;
        hasfocus = true;
    }
    else if (sender() == ui->usblistWidget2)
    {
        current_directory2 = targetDir;
        hasfocus = false;
    }
    else
    {
        return;
    }

    if (filePaths.isEmpty())
        return;

    userpush(filePaths);
}
