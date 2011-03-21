#include "statsdialog.h"
#include "ui_statsdialog.h"
#include <QLabel>
#include <QMap>
#include <QPixmap>
#include <QTextEdit>
#include "libjacksms/libJackSMS.h"
#include <QHBoxLayout>
#include "libjacksms/ConfiguredAccount.h"

StatsDialog::StatsDialog(const libJackSMS::dataTypes::optionsType &_opzioni, const libJackSMS::dataTypes::configuredServicesType &_servizi, const libJackSMS::dataTypes::servicesType &_ElencoServizi, QWidget *parent) :

    QDialog(parent),
    m_ui(new Ui::StatsDialog),
    opzioni(_opzioni),
    servizi(_servizi),
    ElencoServizi(_ElencoServizi)
{
    m_ui->setupUi(this);

    {
        libJackSMS::dataTypes::optionsType::const_iterator i = opzioni.find("sent-count");
        QString n = "0";
        if (i != opzioni.end())
            n = i.value();

        m_ui->labelTotal->setText("Sms inviati da questa postazione: <b>" + n + "</b>");
    }

    QMap<QString, libJackSMS::dataTypes::configuredAccount> tempMap;
    for(libJackSMS::dataTypes::configuredServicesType::const_iterator i = servizi.begin(); i != servizi.end(); ++i) {
        if (i.value().getId() != "1")
            tempMap.insert(i.value().getName(), i.value());
    }

    // Aggiungo Jacksms Messenger separatamente in modo che sia in cima alla lista
    libJackSMS::dataTypes::configuredServicesType::const_iterator i = servizi.find("1");
    QHBoxLayout *hLayout = new QHBoxLayout;
    {
        QLabel *l = new QLabel;
        l->setPixmap(ElencoServizi[i.value().getService()].getIcon().pixmap(16,16));
        l->setFixedSize(16,16);
        hLayout->addWidget(l);
    }
    {
        QTextEdit *l = new QTextEdit;

        QString result="<b>"+i.value().getName()+"</b><br>Inviati oggi da tutti i client: "+i.value().getStat("sent-all")+"<br>Totale inviati da questa postazione: "+i.value().getStat("sent");
        l->setText(result);
        l->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);
        l->setMaximumHeight(50);
        l->setReadOnly(true);
        hLayout->addWidget(l);
    }
    m_ui->layoutStatistiche->addLayout(hLayout);


    for(libJackSMS::dataTypes::configuredServicesType::const_iterator i = tempMap.begin(); i != tempMap.end(); ++i) {
        QHBoxLayout *hLayout = new QHBoxLayout;
        {
            QLabel *l = new QLabel;
            l->setPixmap(ElencoServizi[i.value().getService()].getIcon().pixmap(16,16));
            l->setFixedSize(16,16);
            hLayout->addWidget(l);
        }
        {
            QTextEdit *l = new QTextEdit;

            QString result="<b>"+i.value().getName()+"</b><br>Inviati oggi da tutti i client: "+i.value().getStat("sent-all")+"<br>Totale inviati da questa postazione: "+i.value().getStat("sent");
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
