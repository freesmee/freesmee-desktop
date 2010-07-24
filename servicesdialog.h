#include <QtGui/QDialog>
#include <QListWidgetItem>
#include <QProcess>
#include "mainjacksms.h"
#include "Types.h"
#include "libjacksms/libJackSMS.h"
#include <QMovie>

#ifndef SERVICESDIALOG_H
#define SERVICESDIALOG_H


namespace Ui {
    class ServicesDialog;
}

class ServicesDialog : public QDialog {
    Q_OBJECT
public:
    ServicesDialog(QWidget *parent,MainJackSMS * _padre,libJackSMS::dataTypes::servicesType &_ElencoServizi,libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,const libJackSMS::dataTypes::optionsType &_opzioni);
    ~ServicesDialog();

protected:
    void changeEvent(QEvent *e);

private:
    MainJackSMS *padre;
    Ui::ServicesDialog *m_ui;
    libJackSMS::dataTypes::servicesType &ElencoServizi;
    libJackSMS::dataTypes::configuredServicesType &ElencoServiziConfigurati;
    QString currentId;

    QMovie *spinner;
    libJackSMS::serverApi::accountManager *saver;
    libJackSMS::dataTypes::configuredAccount newAcc;
    const libJackSMS::dataTypes::optionsType &opzioni;
private slots:

    void on_Salva_clicked();
    void on_ListServizi_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void addAccountKo();
    void addAccountOk(QString);
};

#endif // SERVICESDIALOG_H
