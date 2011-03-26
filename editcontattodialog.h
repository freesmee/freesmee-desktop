#include "mainjacksms.h"


#ifndef EDITCONTATTODIALOG_H
#define EDITCONTATTODIALOG_H
#include <QtGui/QDialog>

#include <QMovie>
#include "libjacksms/libJackSMS.h"
namespace Ui {
    class editcontattodialog;
}

class editcontattodialog : public QDialog {
    Q_OBJECT
public:
    editcontattodialog(QWidget *parent, MainJackSMS *_padre, const libJackSMS::dataTypes::servicesType & _ElencoServizi,const libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,libJackSMS::dataTypes::phoneBookType &_Rubrica,QString contactId,const libJackSMS::dataTypes::optionsType _Opzioni);
    ~editcontattodialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::editcontattodialog *m_ui;

    MainJackSMS * padre;
    const libJackSMS::dataTypes::servicesType & ElencoServizi;
    const libJackSMS::dataTypes::configuredServicesType &ElencoServiziConfigurati;
    libJackSMS::dataTypes::phoneBookType &Rubrica;
    libJackSMS::serverApi::contactManager *saver;
    QMovie *spinner;
    const libJackSMS::dataTypes::optionsType Opzioni;
    QString id;
private slots:
    void on_salva_clicked();
    void on_annulla_clicked();

    void salvataggioOk(libJackSMS::dataTypes::contact);
    void salvataggioKo();
};

#endif // EDITCONTATTODIALOG_H
