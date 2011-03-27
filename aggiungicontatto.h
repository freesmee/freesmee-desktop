#include "libjacksms/libJackSMS.h"
#include "Types.h"
#include <QMovie>
#include <QtGui/QDialog>

#ifndef AGGIUNGICONTATTO_H
#define AGGIUNGICONTATTO_H

namespace Ui {
    class AggiungiContatto;
}

class AggiungiContatto : public QDialog {
    Q_OBJECT
public:
    AggiungiContatto(QWidget *parent, libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,libJackSMS::dataTypes::phoneBookType &_Rubrica, libJackSMS::dataTypes::servicesType &_ElencoServizi, libJackSMS::dataTypes::optionsType &_opzioni, QString _current_login_id);
    AggiungiContatto(QWidget *parent, libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,libJackSMS::dataTypes::phoneBookType &_Rubrica, libJackSMS::dataTypes::servicesType &_ElencoServizi, libJackSMS::dataTypes::optionsType &_opzioni, QString _current_login_id, libJackSMS::dataTypes::phoneNumber numero);
    ~AggiungiContatto();

protected:
    void changeEvent(QEvent *e);

private:
    libJackSMS::serverApi::contactManager *saver;
    Ui::AggiungiContatto *m_ui;
    libJackSMS::dataTypes::configuredServicesType &ElencoServiziConfigurati;
    libJackSMS::dataTypes::phoneBookType &Rubrica;
    libJackSMS::dataTypes::servicesType &ElencoServizi;
    QMovie *spinner;
    libJackSMS::dataTypes::optionsType &opzioni;
    QString current_login_id;
    libJackSMS::dataTypes::contact contatto;

private slots:
    void on_salva_clicked();
    void on_annulla_clicked();
    void salvataggioOk(QString, bool);
    void salvataggioKo();

signals:
    void contactAdded(QString);
};

#endif // AGGIUNGICONTATTO_H
