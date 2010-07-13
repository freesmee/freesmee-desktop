#include "aggiungicontatto.h"
#include "ui_aggiungicontatto.h"
#include "mainjacksms.h"
#include "Types.h"
#include <QThread>
#include <QMessageBox>
#include "Configuration.h"
#include "libjacksms/libJackSMS.h"
#include "threadsavecontactonline.h"
AggiungiContatto::AggiungiContatto(QWidget *parent , MainJackSMS * _padre,libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,libJackSMS::dataTypes::phoneBookType &_Rubrica,libJackSMS::dataTypes::servicesType &_ElencoServizi,bool _onlineLogin,libJackSMS::dataTypes::optionsType & _opzioni) :
    QDialog(parent),

    m_ui(new Ui::AggiungiContatto),
    padre(_padre),
    ElencoServiziConfigurati(_ElencoServiziConfigurati),
    Rubrica(_Rubrica),
    ElencoServizi(_ElencoServizi),
    onlineLogin(_onlineLogin),
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
            QIcon ico=ElencoServizi[i.value().getService()].getIcon();
            /*libJackSMS::dataTypes::servicesType::const_iterator x=ElencoServizi.find(i.value().getService());
            if (x!=ElencoServizi.end()){
                if (x->second.getIcon()!=""){
                    try{
                        QByteArray bytes(reinterpret_cast<const char*>(x->second.getIcon().c_str()),x->second.getIcon().length());
                        QImage im=QImage::fromData(bytes,"png");
                        ico=QIcon(QPixmap::fromImage(im));
                    }catch(...){
                        //errore nel recuperare / generare l'icona...
                    }
                }
            }*/



            m_ui->comboaccount->addItem(ico,i.value().getName());
        }
        spinner=new QMovie(":/resource/loading-spinner.gif",QByteArray("gif"),this);
        spinner->setScaledSize(QSize(16,16));
        spinner->start();
        m_ui->labelSpin->setMovie(spinner);
        m_ui->labelSpin->hide();
    }
    /*{
        libJackSMS::dataTypes::phoneBookType::const_iterator i=Rubrica.begin();
        libJackSMS::dataTypes::phoneBookType::const_iterator i_end=Rubrica.end();
        for(;i!=i_end;++i){
            if (m_ui->ComboGruppo->findText(i->gruppo)== -1){
                m_ui->ComboGruppo->addItem(i->gruppo);
            }
        }
    }*/
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

        libJackSMS::dataTypes::contact contatto(nome,num,gruppo,idAccount);
        m_ui->salva->setEnabled(false);
        m_ui->annulla->setEnabled(false);
        if (onlineLogin){
            m_ui->labelSpin->show();



            saver=new threadSaveContactOnline(Rubrica,padre->signin->getSessionId(),contatto,opzioni) ;
            connect(saver,SIGNAL(saveOk()),this,SLOT(salvataggioOk()));
            connect(saver,SIGNAL(saveKo()),this,SLOT(salvataggioKo()));
            saver->start();
        }else{
            libJackSMS::localApi::contactManager manager(padre->current_user_directory);
            if (!manager.addNewContact(contatto)){
                QMessageBox::critical(this,"JackSMS","Errore durante il salvataggio del contatto.");
                m_ui->salva->setEnabled(true);
                m_ui->annulla->setEnabled(true);
            }else{
                padre->ReloadRubrica();
                this->close();
            }
        }



}

void AggiungiContatto::salvataggioOk(){
    padre->ReWriteAddressBookToGui();
    this->close();
}
void AggiungiContatto::salvataggioKo(){
    this->close();
}
void AggiungiContatto::ReadStdoutSaveContact(){

}
