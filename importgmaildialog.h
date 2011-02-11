#ifndef IMPORTGMAILDIALOG_H
#define IMPORTGMAILDIALOG_H

#include <QDialog>
#include "libjacksms/libJackSMS.h"
#include <QMovie>
namespace Ui {
    class importGmailDialog;
}

class importGmailDialog : public QDialog {
    Q_OBJECT
public:
    importGmailDialog(QString _lid,const libJackSMS::dataTypes::optionsType &o,libJackSMS::dataTypes::phoneBookType &_phoneBook,QWidget *parent = 0);
    ~importGmailDialog();

protected:
    void changeEvent(QEvent *e);

private:
    QString logid;
    const libJackSMS::dataTypes::optionsType &opt;

    Ui::importGmailDialog *ui;
    libJackSMS::serverApi::gmailAddressBookImporter *importer;
    libJackSMS::serverApi::reloader *reloader;
    libJackSMS::dataTypes::phoneBookType &phoneBook;
    QMovie* spinner;
private slots:
    void on_importButton_clicked();
    void on_pushButton_clicked();
    void importDone(int);
    void importError(QString e);
    void wrongCredentials();
    void phoneBookReloaded(libJackSMS::dataTypes::phoneBookType);
    void errorReload();
signals:
    void rewritePhoneBook();

};

#endif // IMPORTGMAILDIALOG_H
