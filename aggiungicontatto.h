#include "mainjacksms.h"
#include "Types.h"
#include <QMovie>
#include "libjacksms/libJackSMS.h"
#include "threadsavecontactonline.h"
#ifndef AGGIUNGICONTATTO_H
#define AGGIUNGICONTATTO_H

#include <QtGui/QDialog>

namespace Ui {
    class AggiungiContatto;
}

class AggiungiContatto : public QDialog {
    Q_OBJECT
public:
    AggiungiContatto(QWidget *parent , MainJackSMS * _padre,libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,libJackSMS::dataTypes::phoneBookType &_Rubrica,libJackSMS::dataTypes::servicesType &_ElencoServizi,bool _onlineLogin,libJackSMS::dataTypes::optionsType & _opzioni);

    ~AggiungiContatto();

protected:
    void changeEvent(QEvent *e);

private:
    threadSaveContactOnline *saver;

    Ui::AggiungiContatto *m_ui;
    MainJackSMS * padre;
    libJackSMS::dataTypes::configuredServicesType &ElencoServiziConfigurati;

    libJackSMS::dataTypes::phoneBookType &Rubrica;
    libJackSMS::dataTypes::servicesType &ElencoServizi;
    bool onlineLogin;
    QMovie *spinner;
    libJackSMS::dataTypes::optionsType & opzioni;
private slots:
    void on_salva_clicked();
    void on_annulla_clicked();
    void ReadStdoutSaveContact();
    void salvataggioOk();
    void salvataggioKo();
};

#endif // AGGIUNGICONTATTO_H
