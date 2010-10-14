#include "statsdialog.h"
#include "ui_statsdialog.h"
#include <QLabel>
#include <QMap>
#include <QPixmap>
#include <QTextEdit>
#include "libjacksms/libJackSMS.h"
#include <QHBoxLayout>
StatsDialog::StatsDialog(const libJackSMS::dataTypes::optionsType & _opzioni,const libJackSMS::dataTypes::configuredServicesType & _servizi,const libJackSMS::dataTypes::servicesType &_ElencoServizi,QWidget *parent) :

    QDialog(parent),
    m_ui(new Ui::StatsDialog),
    opzioni(_opzioni),
    servizi(_servizi),
    ElencoServizi(_ElencoServizi)
{
    m_ui->setupUi(this);

    {
        libJackSMS::dataTypes::optionsType::const_iterator i=opzioni.find("sent-count");
        QString n="0";
        if (i!=opzioni.end())
            n=i.value();

        m_ui->labelTotal->setText("Sms inviati con JackSMS Desktop: <b>"+n+"</b>");
    }
    libJackSMS::dataTypes::configuredServicesType::const_iterator i=servizi.begin();
    libJackSMS::dataTypes::configuredServicesType::const_iterator i_end=servizi.end();
    for(;i!=i_end;++i){
        QHBoxLayout *hLayout = new QHBoxLayout;
        {
            QLabel *l=new QLabel;
            /*QMap<QString,QPixmap>::const_iterator x=Icone.find(i.value().getService());
            if (x!=Icone.end())
                l->setPixmap(x.value());
*/
            l->setPixmap(ElencoServizi[i.value().getService()].getIcon().pixmap(16,16));
            l->setMaximumSize(16,16);
            l->setMinimumSize(16,16);
            hLayout->addWidget(l);
        }
        {
            QTextEdit *l=new QTextEdit;

            QString result="<b>"+i.value().getName()+"</b><br>Inviati con JackSMS Desktop: "+i.value().getStat("sent")+"<br>Inviati in totale con "+i.value().getName()+": "+i.value().getStat("sent-all");
            /*
            QString id=i.value().getService();

            QString reset=_ElencoServizi[id].getReset();
            if (reset.toUpper()=="DAILY"){
                result=result.append("<br>Inviati oggi: "+i.value().getStat("sent-partial"));
            }else if (reset.toUpper()=="MONTHLY"){
                result=result.append("<br>Inviati in questo mese: "+i.value().getStat("sent-partial"));
            }else if (reset.toUpper()=="WEEKLY"){
                result=result.append("<br>Inviati in questa settimana: "+i.value().getStat("sent-partial"));
            }*/

            l->setText(result);
            l->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);
            l->setMaximumHeight(50);
            l->setReadOnly(true);
            hLayout->addWidget(l);
        }
        m_ui->layoutStatistiche->addLayout(hLayout);
    }

}

StatsDialog::~StatsDialog()
{
    delete m_ui;
}

void StatsDialog::changeEvent(QEvent *e)
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
