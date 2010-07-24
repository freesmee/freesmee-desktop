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
        libJackSMS::dataTypes::configuredServicesType::const_iterator i=ElencoServiziConfigurati.begin();
        libJackSMS::dataTypes::configuredServicesType::const_iterator i_end=ElencoServiziConfigurati.end();
        for(;i!=i_end;++i){
            m_ui->comboaccount->addItem(_ElencoServizi[i->getService()].getIcon(),i->getName());
        }

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
    num.parse(m_ui->intPref->text()+"."+m_ui->pref->text()+"."+m_ui->num->text());
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
    Rubrica[c.getId()]=c;
    padre->ReWriteAddressBookToGui();
    this->close();
}
void editcontattodialog::salvataggioKo(){
    this->close();
}
