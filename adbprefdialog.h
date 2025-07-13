#ifndef ADBPREFDIALOG_H
#define ADBPREFDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class QLabel;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;

class adbprefDialog : public QDialog
{
    Q_OBJECT
public:
    explicit adbprefDialog(QWidget *parent = nullptr);
    ~adbprefDialog() override;

    // Getters
    QString downloaddir() const;
    QString installdir() const;
    QString backupdir() const;
    QString localadb() const;
    QString donation() const;
    bool versioncheck() const;
    bool scrcpyargs() const;
    bool startview() const;
    int defaultwindow() const; // Changed from bool to int
    int linterm() const;
    int macterm() const;

    // Setters
    void setdownloaddir(const QString &dir);
    void setinstalldir(const QString &dir);
    void setbackupdir(const QString &dir);
    void setlocaladb(const QString &path);
    void setdonation(const QString &code);
    void setversioncheck(bool val);
    void setscrcpyargs(bool val);
    void setstartview(bool val);
    void setdefaultwindow(int index); // Changed from bool to int
    void setlinterm(int index);
    void setmacterm(int index);

protected:
    void accept() override;

private slots:
    void on_checkButton_clicked();
    void onRequestCompleted();
    void on_downloadButton_clicked();
    void on_installButton_clicked();
    void on_backupButton_clicked();
    void on_adbButton_clicked();

private:
    void setupUiManual();

    QCheckBox *versionCheckBox;
    QCheckBox *scrcpyArgsCheckBox;
    QCheckBox *startViewCheckBox;
    QComboBox *defaultWindowCombo;
    QComboBox *lgfontCombo;
    QComboBox *mdfontCombo;
    QComboBox *smfontCombo;

    QComboBox *macTermCombo;
    QComboBox *linTermCombo;

    QLineEdit *donationEdit;
    QLineEdit *downloadPathEdit;
    QLineEdit *installPathEdit;
    QLineEdit *backupPathEdit;
    QLineEdit *localAdbEdit;

    QPushButton *cancelButton;
    QPushButton *checkButton;
    QPushButton *okButton;

    QPushButton *downloadButton;
    QPushButton *installButton;
    QPushButton *backupButton;
    QPushButton *adbButton;

    QNetworkAccessManager *m_networkManager;
    const QString version2 = "6.7";
};

#endif // ADBPREFDIALOG_H
