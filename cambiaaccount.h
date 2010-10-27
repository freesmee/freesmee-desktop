#include "mainjacksms.h"

#ifndef CAMBIAACCOUNT_H
#define CAMBIAACCOUNT_H

#include <QDialog>
#include <libjacksms/libJackSMS.h>


namespace Ui {
    class cambiaaccount;
}

class cambiaaccount : public QDialog
{
    Q_OBJECT

public:
    cambiaaccount(QWidget *parent, MainJackSMS * _padre, const libJackSMS::dataTypes::servicesType & _ElencoServizi, const libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati, libJackSMS::dataTypes::phoneBookType &_Rubrica, QString _id, int _found);
    ~cambiaaccount();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::cambiaaccount *m_ui;

    MainJackSMS * padre;
    const libJackSMS::dataTypes::servicesType & ElencoServizi;
    const libJackSMS::dataTypes::configuredServicesType &ElencoServiziConfigurati;
    libJackSMS::dataTypes::phoneBookType &Rubrica;
    libJackSMS::serverApi::contactManager *saver;
    QMovie *spinner;
    QString id;
    int found;

private slots:
    void on_buttonAnnulla_clicked();
    void on_buttonSalva_clicked();
};

#endif // CAMBIAACCOUNT_H
