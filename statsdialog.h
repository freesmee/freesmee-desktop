#ifndef STATSDIALOG_H
#define STATSDIALOG_H

#include <QtGui/QDialog>
#include "libjacksms/libJackSMS.h"
namespace Ui {
    class StatsDialog;
}

class StatsDialog : public QDialog {
    Q_OBJECT
public:
    StatsDialog(const libJackSMS::dataTypes::optionsType & _opzioni,const libJackSMS::dataTypes::configuredServicesType & _servizi,const libJackSMS::dataTypes::servicesType &_ElencoServizi,QWidget *parent = 0);
    ~StatsDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::StatsDialog *m_ui;
    const libJackSMS::dataTypes::optionsType & opzioni;
    const libJackSMS::dataTypes::configuredServicesType & servizi;
    const libJackSMS::dataTypes::servicesType &ElencoServizi;
};

#endif // STATSDIALOG_H
