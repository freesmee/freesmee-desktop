#include "mainjacksms.h"


#ifndef EDITCONTATTODIALOG_H
#define EDITCONTATTODIALOG_H
#include "threadupdatecontact.h"
#include <QtGui/QDialog>

#include <QMovie>
namespace Ui {
    class editcontattodialog;
}

class editcontattodialog : public QDialog {
    Q_OBJECT
public:
    editcontattodialog(QWidget *parent , MainJackSMS * _padre,const libJackSMS::dataTypes::servicesType & _ElencoServizi,const libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,libJackSMS::dataTypes::phoneBookType &_Rubrica,QString contactId,const libJackSMS::dataTypes::optionsType _Opzioni);
    ~editcontattodialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::editcontattodialog *m_ui;

    MainJackSMS * padre;
    const libJackSMS::dataTypes::servicesType & ElencoServizi;
    const libJackSMS::dataTypes::configuredServicesType &ElencoServiziConfigurati;
    libJackSMS::dataTypes::phoneBookType &Rubrica;
    threadUpdateContact *saver;
    QMovie *spinner;
    const libJackSMS::dataTypes::optionsType Opzioni;
    QString id;
private slots:
    void on_salva_clicked();
    void on_annulla_clicked();

    void salvataggioOk();
    void salvataggioKo();
};

#endif // EDITCONTATTODIALOG_H
