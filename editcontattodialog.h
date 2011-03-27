#include "libjacksms/libJackSMS.h"
#include <QtGui/QDialog>
#include <QMovie>

#ifndef EDITCONTATTODIALOG_H
#define EDITCONTATTODIALOG_H

namespace Ui {
    class editcontattodialog;
}

class editcontattodialog : public QDialog {
    Q_OBJECT
public:
    editcontattodialog(QWidget *parent, const libJackSMS::dataTypes::servicesType & _ElencoServizi, const libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati, libJackSMS::dataTypes::phoneBookType &_Rubrica, QString contactId, const libJackSMS::dataTypes::optionsType _Opzioni, QString _current_login_id);
    ~editcontattodialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::editcontattodialog *m_ui;
    const libJackSMS::dataTypes::servicesType &ElencoServizi;
    const libJackSMS::dataTypes::configuredServicesType &ElencoServiziConfigurati;
    libJackSMS::dataTypes::phoneBookType &Rubrica;
    libJackSMS::serverApi::contactManager *saver;
    QMovie *spinner;
    const libJackSMS::dataTypes::optionsType Opzioni;
    QString id;
    QString current_login_id;

private slots:
    void on_salva_clicked();
    void on_annulla_clicked();
    void salvataggioOk(libJackSMS::dataTypes::contact);
    void salvataggioKo();

signals:
    void contactEdited(QString);

};

#endif // EDITCONTATTODIALOG_H
