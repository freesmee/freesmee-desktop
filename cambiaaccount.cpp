#include "cambiaaccount.h"
#include "ui_cambiaaccount.h"
#include <libjacksms/libJackSMS.h>

cambiaaccount::cambiaaccount(QWidget *parent, MainJackSMS * _padre, const libJackSMS::dataTypes::servicesType & _ElencoServizi, const libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati, libJackSMS::dataTypes::phoneBookType &_Rubrica, QString _id, int _found) :
    QDialog(parent),
    padre(_padre),
    ElencoServizi(_ElencoServizi),
    ElencoServiziConfigurati(_ElencoServiziConfigurati),
    Rubrica(_Rubrica),
    id(_id),
    found(_found),
    m_ui(new Ui::cambiaaccount)
{
    m_ui->setupUi(this);

    m_ui->label->setText("Sono stati trovati " + QString::number(found) + " contatti aventi il servizio cancellato come account associato. Si possono associare automaticamente questi contatti a un nuovo servizio.");

    spinner=new QMovie(":/resource/loading-spinner.gif",QByteArray("gif"),this);
    spinner->setScaledSize(QSize(16,16));
    spinner->start();
    m_ui->labelSpin->setMovie(spinner);
    m_ui->labelSpin->hide();

    {
        libJackSMS::dataTypes::configuredServicesType::const_iterator i=ElencoServiziConfigurati.begin();
        libJackSMS::dataTypes::configuredServicesType::const_iterator i_end=ElencoServiziConfigurati.end();
        for(;i!=i_end;++i){
            m_ui->comboaccount->addItem(_ElencoServizi[i->getService()].getIcon(),i->getName());
        }
        m_ui->comboaccount->model()->sort(0);
    }

}

cambiaaccount::~cambiaaccount()
{
    delete m_ui;
}

void cambiaaccount::on_buttonSalva_clicked()
{
    m_ui->labelSpin->show();

    this->close();
}

void cambiaaccount::on_buttonAnnulla_clicked()
{
    this->close();
}


void cambiaaccount::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
