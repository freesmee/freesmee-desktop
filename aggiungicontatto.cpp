#include "aggiungicontatto.h"
#include "ui_aggiungicontatto.h"
#include "Types.h"
#include <QThread>
#include <QMessageBox>
#include "Configuration.h"
#include "libjacksms/libJackSMS.h"
#include "mainjacksms.h"
#include <QTextCursor>

AggiungiContatto::AggiungiContatto(QWidget *parent, libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati, libJackSMS::dataTypes::phoneBookType &_Rubrica, libJackSMS::dataTypes::servicesType &_ElencoServizi, libJackSMS::dataTypes::optionsType &_opzioni, QString _current_login_id) :
    QDialog(parent),
    m_ui(new Ui::AggiungiContatto),
    ElencoServiziConfigurati(_ElencoServiziConfigurati),
    Rubrica(_Rubrica),
    ElencoServizi(_ElencoServizi),
    opzioni(_opzioni),
    current_login_id(_current_login_id)
{
    m_ui->setupUi(this);
    m_ui->ComboGruppo->hide();
    m_ui->nuovogruppo->hide();
    m_ui->radioEsistente->hide();
    m_ui->radioNuovo->hide();
    {
        for (libJackSMS::dataTypes::configuredServicesType::const_iterator i = ElencoServiziConfigurati.begin(); i != ElencoServiziConfigurati.end(); ++i) {
            if (i.value().getId() != "1") {
                m_ui->comboaccount->addItem(ElencoServizi[i.value().getService()].getIcon(),i.value().getName());
            }
        }
        m_ui->comboaccount->model()->sort(0);
        m_ui->comboaccount->insertItem(0, ElencoServizi[ElencoServiziConfigurati["1"].getService()].getIcon(), ElencoServiziConfigurati["1"].getName());
        m_ui->comboaccount->setCurrentIndex(0);

        spinner = new QMovie(":/resource/loading-spinner.gif", QByteArray("gif"), this);
        spinner->setScaledSize(QSize(16,16));
        spinner->start();
        m_ui->labelSpin->setMovie(spinner);
        m_ui->labelSpin->hide();
    }
}

//questo è il costruttore per quando si usa il "Salva contatto"
AggiungiContatto::AggiungiContatto(QWidget *parent, libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati, libJackSMS::dataTypes::phoneBookType &_Rubrica, libJackSMS::dataTypes::servicesType &_ElencoServizi, libJackSMS::dataTypes::optionsType &_opzioni, QString _current_login_id, libJackSMS::dataTypes::phoneNumber numero) :
        QDialog(parent),
        m_ui(new Ui::AggiungiContatto),
        ElencoServiziConfigurati(_ElencoServiziConfigurati),
        Rubrica(_Rubrica),
        ElencoServizi(_ElencoServizi),
        opzioni(_opzioni),
        current_login_id(_current_login_id)
{
    m_ui->setupUi(this);
    m_ui->ComboGruppo->hide();
    m_ui->nuovogruppo->hide();
    m_ui->radioEsistente->hide();
    m_ui->radioNuovo->hide();
    {
        if (numero.getVirtual()) {

            m_ui->comboaccount->addItem(ElencoServizi[ElencoServiziConfigurati["1"].getService()].getIcon(), ElencoServiziConfigurati["1"].getName());
            m_ui->comboaccount->setCurrentIndex(0);
            m_ui->comboaccount->setEnabled(false);

        } else {

            for (libJackSMS::dataTypes::configuredServicesType::const_iterator i = ElencoServiziConfigurati.begin(); i != ElencoServiziConfigurati.end(); ++i) {
                if (i.value().getId() != "1") {
                    m_ui->comboaccount->addItem(ElencoServizi[i.value().getService()].getIcon(),i.value().getName());
                }
            }

            m_ui->comboaccount->model()->sort(0);
            m_ui->comboaccount->insertItem(0, ElencoServizi[ElencoServiziConfigurati["1"].getService()].getIcon(), ElencoServiziConfigurati["1"].getName());
            m_ui->comboaccount->setCurrentIndex(0);
            m_ui->comboaccount->setEnabled(true);

        }

        spinner = new QMovie(":/resource/loading-spinner.gif", QByteArray("gif"), this);
        spinner->setScaledSize(QSize(16,16));
        spinner->start();
        m_ui->labelSpin->setMovie(spinner);
        m_ui->labelSpin->hide();
    }

    m_ui->nome->setText(numero.getAltName());
    m_ui->intPref->setText(numero.getIntPref());
    m_ui->pref->setText(numero.getPref());
    m_ui->num->setText(numero.getNum());
    m_ui->intPref->setEnabled(false);
    m_ui->pref->setEnabled(false);
    m_ui->num->setEnabled(false);
}

AggiungiContatto::~AggiungiContatto()
{
    delete m_ui;
}

void AggiungiContatto::changeEvent(QEvent *e)
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

void AggiungiContatto::on_annulla_clicked()
{
    close();
}

void AggiungiContatto::on_salva_clicked()
{
        QString nome = m_ui->nome->text();
        libJackSMS::dataTypes::phoneNumber num;
        bool numeroValido;
        numeroValido = num.parse((m_ui->intPref->text().isEmpty() ? "" : m_ui->intPref->text() + ".") + m_ui->pref->text() + "." + m_ui->num->text());
        if(!numeroValido){
            QMessageBox::critical(this,"Attenzione","Il numero inserito non è valido, ricontrollalo, oppure se trovi che questo sia un errore segnalalo, grazie.");
            return;
        }

        QString gruppo;
        if (m_ui->radioNuovo->isChecked()) {
            QRegExp checker;
            checker.setPattern("^[a-zA-Z0-9_]*$");
            if (checker.exactMatch(m_ui->nuovogruppo->text())) {
                gruppo = m_ui->nuovogruppo->text();
            }else{
                QMessageBox::critical(this, "Errore", "Il nome del gruppo contiene caratteri non supportati (solo lettere, numeri e underscore)");
                return;
            }
        } else {
            gruppo = m_ui->ComboGruppo->currentText();
        }
        QString idAccount = "";

        {
            QString nomeAccount = m_ui->comboaccount->currentText();
            for(libJackSMS::dataTypes::configuredServicesType::const_iterator i = ElencoServiziConfigurati.begin(); i != ElencoServiziConfigurati.end(); ++i) {
                if (i.value().getName() == nomeAccount) {
                    idAccount=i.value().getId();
                    break;
                }
            }
        }
        //QString nn=nome.to;

        contatto = libJackSMS::dataTypes::contact(nome, num, gruppo, idAccount);
        m_ui->salva->setEnabled(false);
        m_ui->annulla->setEnabled(false);
        m_ui->labelSpin->show();

        saver = new libJackSMS::serverApi::contactManager(current_login_id, opzioni);
        connect(saver, SIGNAL(contactSaved(QString, bool)), this, SLOT(salvataggioOk(QString, bool)));
        connect(saver, SIGNAL(contactNotSaved()), this, SLOT(salvataggioKo()));
        saver->addNewContact(contatto);
}

void AggiungiContatto::salvataggioOk(QString id, bool jms) {
    contatto.setId(id);
    contatto.setCanReceiveJms(jms);
    Rubrica.insert(id, contatto);
    emit contactAdded(id);
    close();
}

void AggiungiContatto::salvataggioKo() {
    m_ui->salva->setEnabled(true);
    m_ui->annulla->setEnabled(true);
    m_ui->labelSpin->hide();
    QMessageBox::critical(this, "JackSMS", "Si e' verificato un errore durante il salvataggio del contatto.");
}
