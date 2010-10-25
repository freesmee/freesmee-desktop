#include "aggiungicontatto.h"
#include "ui_aggiungicontatto.h"
#include "mainjacksms.h"
#include "Types.h"
#include <QThread>
#include <QMessageBox>
#include "Configuration.h"
#include "libjacksms/libJackSMS.h"

AggiungiContatto::AggiungiContatto(QWidget *parent , MainJackSMS * _padre,libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,libJackSMS::dataTypes::phoneBookType &_Rubrica,libJackSMS::dataTypes::servicesType &_ElencoServizi,libJackSMS::dataTypes::optionsType & _opzioni) :
    QDialog(parent),

    m_ui(new Ui::AggiungiContatto),
    padre(_padre),
    ElencoServiziConfigurati(_ElencoServiziConfigurati),
    Rubrica(_Rubrica),
    ElencoServizi(_ElencoServizi),
    opzioni(_opzioni)
{
    m_ui->setupUi(this);
    m_ui->ComboGruppo->hide();
    m_ui->nuovogruppo->hide();
    m_ui->radioEsistente->hide();
    m_ui->radioNuovo->hide();
    {
        libJackSMS::dataTypes::configuredServicesType::const_iterator i=ElencoServiziConfigurati.begin();
        libJackSMS::dataTypes::configuredServicesType::const_iterator i_end=ElencoServiziConfigurati.end();
        for(;i!=i_end;++i){
            m_ui->comboaccount->addItem(ElencoServizi[i.value().getService()].getIcon(),i.value().getName());
        }
        m_ui->comboaccount->model()->sort(0);
        spinner=new QMovie(":/resource/loading-spinner.gif",QByteArray("gif"),this);
        spinner->setScaledSize(QSize(16,16));
        spinner->start();
        m_ui->labelSpin->setMovie(spinner);
        m_ui->labelSpin->hide();
    }
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
    this->close();
}

void AggiungiContatto::on_salva_clicked()
{
        QString nome=m_ui->nome->text();
        libJackSMS::dataTypes::phoneNumber num;
        num.parse(m_ui->intPref->text()+"."+m_ui->pref->text()+"."+m_ui->num->text());

        QString gruppo;
        if (m_ui->radioNuovo->isChecked()){
            QRegExp checker;
            checker.setPattern("^[a-zA-Z0-9_]*$");
            if (checker.exactMatch(m_ui->nuovogruppo->text())){
                gruppo=m_ui->nuovogruppo->text();
            }else{
                QMessageBox::critical(this,"Errore","Il nome del gruppo contiene caratteri non supportati (solo lettere, numeri e underscore)");
                return;
            }
        }else{
            gruppo=m_ui->ComboGruppo->currentText();
        }
        QString idAccount;
        {

            QString nomeAccount=m_ui->comboaccount->currentText();
            libJackSMS::dataTypes::configuredServicesType::const_iterator i=ElencoServiziConfigurati.begin();
            libJackSMS::dataTypes::configuredServicesType::const_iterator i_end=ElencoServiziConfigurati.end();

            for(;i!=i_end;++i){
                if (i.value().getName()==nomeAccount){
                    idAccount=i.value().getId();

                    break;

                }
            }
        }
        //QString nn=nome.to;

        contatto=libJackSMS::dataTypes::contact(nome,num,gruppo,idAccount);
        m_ui->salva->setEnabled(false);
        m_ui->annulla->setEnabled(false);

        m_ui->labelSpin->show();



        saver=new libJackSMS::serverApi::contactManager(padre->current_login_id,opzioni);
        connect(saver,SIGNAL(contactSaved(QString,bool)),this,SLOT(salvataggioOk(QString,bool)));
        connect(saver,SIGNAL(contactNotSaved()),this,SLOT(salvataggioKo()));
        saver->addNewContact(contatto);




}

void AggiungiContatto::salvataggioOk(QString id,bool jms){
    contatto.setId(id);
    contatto.setCanReceiveJms(jms);
    Rubrica.insert(id,contatto);
    padre->ReWriteAddressBookToGui();
    padre->ricaricaDestinatariList();
    close();
}
void AggiungiContatto::salvataggioKo(){
    m_ui->salva->setEnabled(true);
    m_ui->annulla->setEnabled(true);
    m_ui->labelSpin->hide();
    QMessageBox::critical(this,"JackSMS","Si e' verificato un errore durante il salvataggio del contatto.");
}
