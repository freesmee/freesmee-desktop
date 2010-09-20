#ifndef EDITACCOUNTDIALOG_H
#define EDITACCOUNTDIALOG_H

#include <QDialog>
#include <libjacksms/libJackSMS.h>
namespace Ui {
    class editAccountDialog;
}

class editAccountDialog : public QDialog {
    Q_OBJECT
public:
    editAccountDialog(libJackSMS::dataTypes::configuredServicesType &_acc, libJackSMS::dataTypes::servicesType &_services ,QString id,QString current_login_id,libJackSMS::dataTypes::optionsType &_opzioni,QWidget *parent = 0);
    ~editAccountDialog();

protected:
    void changeEvent(QEvent *e);

private:
    libJackSMS::dataTypes::configuredServicesType &accounts;
    libJackSMS::dataTypes::servicesType &services;
    QString id;
    QString current_login_id;
    libJackSMS::dataTypes::optionsType &opzioni;
    libJackSMS::dataTypes::configuredAccount currentAccount;

    Ui::editAccountDialog *ui;
    libJackSMS::serverApi::accountManager *saver;

private slots:
    void on_pushButton_clicked();
    void on_buttonAnnulla_clicked();
void updateOk(libJackSMS::dataTypes::configuredAccount);
void accountNotUpdated();
};

#endif // EDITACCOUNTDIALOG_H
