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
    cambiaaccount(QWidget *parent, MainJackSMS * _padre, const libJackSMS::dataTypes::servicesType & _ElencoServizi, const libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati, libJackSMS::dataTypes::phoneBookType &_Rubrica, const libJackSMS::dataTypes::optionsType _Opzioni, QString _id, int _found);
    ~cambiaaccount();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::cambiaaccount *m_ui;

    MainJackSMS * padre;
    const libJackSMS::dataTypes::servicesType & ElencoServizi;
    const libJackSMS::dataTypes::configuredServicesType &ElencoServiziConfigurati;
    libJackSMS::dataTypes::phoneBookType &Rubrica;
    const libJackSMS::dataTypes::optionsType Opzioni;
    libJackSMS::serverApi::contactManager *saver;
    QMovie *spinner;
    QString id;
    int found;
    int changed;
    bool stopsearch;

private slots:
    void on_buttonAnnulla_clicked();
    void on_buttonSalva_clicked();
    void salvataggioKo();
    void salvataggioOk(libJackSMS::dataTypes::contact c);
};

#endif // CAMBIAACCOUNT_H
