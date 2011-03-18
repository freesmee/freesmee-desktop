#include "editcontattodialog.h"
#include "ui_editcontattodialog.h"
#include <QProcess>
#include <Configuration.h>
#include <QMessageBox>


editcontattodialog::editcontattodialog(QWidget *parent , MainJackSMS * _padre,const libJackSMS::dataTypes::servicesType & _ElencoServizi,const libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,libJackSMS::dataTypes::phoneBookType &_Rubrica,QString contactId,const libJackSMS::dataTypes::optionsType _Opzioni) :
    QDialog(parent),
    m_ui(new Ui::editcontattodialog),
    padre(_padre),
    ElencoServizi(_ElencoServizi),
    ElencoServiziConfigurati(_ElencoServiziConfigurati),
    Rubrica(_Rubrica),
    Opzioni(_Opzioni),
    id(contactId)
{

    m_ui->setupUi(this);

    spinner=new QMovie(":/resource/loading-spinner.gif",QByteArray("gif"),this);
    spinner->setScaledSize(QSize(16,16));
    spinner->start();
    m_ui->labelSpin->setMovie(spinner);
    m_ui->labelSpin->hide();

    m_ui->radioEsistente->hide();
    m_ui->radioNuovo->hide();
    m_ui->ComboGruppo->hide();
    m_ui->nuovogruppo->hide();


    libJackSMS::dataTypes::contact contatto(Rubrica[contactId]);

    {
        for (libJackSMS::dataTypes::configuredServicesType::const_iterator i = ElencoServiziConfigurati.begin(); i != ElencoServiziConfigurati.end(); ++i) {
            if (i.value().getId() != "0") {
                m_ui->comboaccount->addItem(ElencoServizi[i.value().getService()].getIcon(),i.value().getName());
            }
        }
        m_ui->comboaccount->model()->sort(0);
        m_ui->comboaccount->insertItem(0, ElencoServizi[ElencoServiziConfigurati["0"].getService()].getIcon(), ElencoServiziConfigurati["0"].getName());
    }

    m_ui->nome->setText(contatto.getName());
    m_ui->intPref->setText(contatto.getPhone().getIntPref());
    m_ui->pref->setText(contatto.getPhone().getPref());
    m_ui->num->setText(contatto.getPhone().getNum());
    //m_ui->ComboGruppo->setCurrentIndex(m_ui->ComboGruppo->findText(Rubrica[nameContact].gruppo));
    m_ui->comboaccount->setCurrentIndex(m_ui->comboaccount->findText(ElencoServiziConfigurati[contatto.getAccount()].getName()));

}
editcontattodialog::~editcontattodialog()
{
    delete m_ui;
}

void editcontattodialog::changeEvent(QEvent *e)
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

void editcontattodialog::on_annulla_clicked()
{
    this->close();
}

void editcontattodialog::on_salva_clicked()
{
    QString nome=m_ui->nome->text();
    libJackSMS::dataTypes::phoneNumber num;
    bool numeroValido;
    numeroValido = num.parse( (m_ui->intPref->text().isEmpty() ? "" : m_ui->intPref->text() + ".") + m_ui->pref->text() + "." + m_ui->num->text());
    if(!numeroValido){
        QMessageBox::critical(this,"Attenzione","Il numero inserito non � valido, ricontrollalo, oppure se trovi che questo sia un errore segnalalo, grazie.");
        return;
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

    libJackSMS::dataTypes::contact contatto(nome,num,"",idAccount);
    contatto.setId(id);
    m_ui->salva->setEnabled(false);
    m_ui->annulla->setEnabled(false);
    m_ui->labelSpin->show();



    saver=new libJackSMS::serverApi::contactManager(padre->current_login_id,Opzioni);
    connect(saver,SIGNAL(contactUpdated(libJackSMS::dataTypes::contact)),this,SLOT(salvataggioOk(libJackSMS::dataTypes::contact)));
    connect(saver,SIGNAL(contactNotUpdated()),this,SLOT(salvataggioKo()));
    saver->updateContact(contatto);


}

void editcontattodialog::salvataggioOk(libJackSMS::dataTypes::contact c){
    Rubrica[c.getId()] = c;
    padre->ReWriteAddressBookToGui();
    padre->updateSmsListAfterContactEdited(c);
    close();
}
void editcontattodialog::salvataggioKo(){
    m_ui->salva->setEnabled(true);
    m_ui->annulla->setEnabled(true);
    m_ui->labelSpin->hide();
    QMessageBox::critical(this,"JackSMS","Si e' verificato un errore durante l'aggiornamento del contatto.");
}

