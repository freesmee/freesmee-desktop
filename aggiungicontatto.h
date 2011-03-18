#include "mainjacksms.h"
#include "Types.h"
#include <QMovie>
#include "libjacksms/libJackSMS.h"

#ifndef AGGIUNGICONTATTO_H
#define AGGIUNGICONTATTO_H

#include <QtGui/QDialog>

namespace Ui {
    class AggiungiContatto;
}

class AggiungiContatto : public QDialog {
    Q_OBJECT
public:
    AggiungiContatto(QWidget *parent , MainJackSMS *_padre, libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,libJackSMS::dataTypes::phoneBookType &_Rubrica, libJackSMS::dataTypes::servicesType &_ElencoServizi, libJackSMS::dataTypes::optionsType & _opzioni);
    AggiungiContatto(QWidget *parent , MainJackSMS *_padre, libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,libJackSMS::dataTypes::phoneBookType &_Rubrica, libJackSMS::dataTypes::servicesType &_ElencoServizi, libJackSMS::dataTypes::optionsType & _opzioni, QString nome, libJackSMS::dataTypes::phoneNumber numero, bool onlyJMS);
    ~AggiungiContatto();

protected:
    void changeEvent(QEvent *e);

private:
    libJackSMS::serverApi::contactManager *saver;

    Ui::AggiungiContatto *m_ui;
    MainJackSMS * padre;
    libJackSMS::dataTypes::configuredServicesType &ElencoServiziConfigurati;

    libJackSMS::dataTypes::phoneBookType &Rubrica;
    libJackSMS::dataTypes::servicesType &ElencoServizi;
    QMovie *spinner;
    libJackSMS::dataTypes::optionsType & opzioni;
    libJackSMS::dataTypes::contact contatto;
private slots:
    void on_salva_clicked();
    void on_annulla_clicked();
    void salvataggioOk(QString,bool);
    void salvataggioKo();
};

#endif // AGGIUNGICONTATTO_H
