#include "opzionidialog.h"
#include "ui_opzionidialog.h"
#include <QListWidgetItem>
#include <QTextEdit>
#include <QMessageBox>
#include "Configuration.h"
#include "JackUtils.h"
#include <QTimer>

OpzioniDialog::OpzioniDialog(libJackSMS::dataTypes::optionsType & _opt,QTextEdit &_TextSms,QString _userDirectory,QWidget *parent,const bool _loggedIn) :

    QDialog(parent),
    opt(_opt),
    TextSms(_TextSms),
    userDirectory(_userDirectory),
    loggedIn(_loggedIn),
    m_ui(new Ui::OpzioniDialog)
{
    m_ui->setupUi(this);


    libJackSMS::dataTypes::optionsType::const_iterator iter;

    // !!!!!!!!!!!!!!!!! disabilito momentaneamente la scheda "lingua"
   m_ui->listWidget->item(5)->~QListWidgetItem();
   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

    if (!loggedIn){
        m_ui->listWidget->item(4)->~QListWidgetItem();
        m_ui->listWidget->item(3)->~QListWidgetItem();
        m_ui->listWidget->item(2)->~QListWidgetItem();
        m_ui->listWidget->item(0)->~QListWidgetItem();

        m_ui->stackedWidget->setCurrentIndex(1);
    }else{
        /*****ATTENZIONE: rimuovo momentaneamente il widget. in versioni superiori verr� reintrodotto*/
        m_ui->listWidget->item(4)->~QListWidgetItem();
        /******/

        /*iter=opt.find("set-bgcolor");
        if (iter!=opt.end())
            if ("yes"==iter.value())
                m_ui->settaColoreBackground->setChecked(true);
*/

        iter=opt.find("set-account");
        if (iter!=opt.end())
            if ("yes"==iter.value())
                m_ui->checkAccountDefault->setChecked(true);
        iter=opt.find("save-local");
        if (iter!=opt.end())
            if ("yes"==iter.value())
                m_ui->checkSalvalocale->setChecked(true);

        iter=opt.find("receive-im");
        if (iter!=opt.end()){
            if ("no"!=iter.value())
                m_ui->CheckAbilitaIM->setChecked(true);
        }else
            m_ui->CheckAbilitaIM->setChecked(true);
        iter=opt.find("use-captcha");
        if (iter!=opt.end())
            if ("yes"==iter.value())
                m_ui->checkUseCaptcha->setChecked(true);

        /*iter=opt.find("receive-interval");
        if (iter!=opt.end())
            m_ui->ComboIntervallo->setCurrentIndex(m_ui->ComboIntervallo->findText(opt["receive-interval"]));
*/
        iter=opt.find("captcha-zoom");
        if (iter!=opt.end()){
            int index=m_ui->comboZoomCaptcha->findText(opt["captcha-zoom"]);

            m_ui->comboZoomCaptcha->setCurrentIndex((index==-1)?0:index);
        }



        iter=opt.find("textsize");
            if (iter!=opt.end())
                m_ui->ComboDimensione->setCurrentIndex(m_ui->ComboDimensione->findText(opt["textsize"]));

            m_ui->stackedWidget->setCurrentIndex(0);
    }
    iter=opt.find("use-proxy");
    if (iter!=opt.end())
        if (iter.value()=="yes")
            m_ui->CheckUsaProxy->setChecked(true);

    iter=opt.find("use-proxy-auth");
    if (iter!=opt.end())
        if (iter.value()=="yes")
            m_ui->usaAutenticazione->setChecked(true);
    iter=opt.find("proxy-port");
    if (iter!=opt.end())
        m_ui->TextPort->setText(iter.value());
    iter=opt.find("proxy-username");
    if (iter!=opt.end())
        m_ui->proxyUsername->setText(iter.value());
    iter=opt.find("proxy-password");
    if (iter!=opt.end())
        m_ui->proxyPassword->setText(iter.value());
    iter=opt.find("proxy");
    if (iter!=opt.end())
        m_ui->TextServer->setText(iter.value());
    iter=opt.find("proxy-type");
    if (iter!=opt.end()){
        if (opt["proxy-type"]=="http")
            m_ui->RadioHttp->setChecked(true);
        else if (opt["proxy-type"]=="socks5")
            m_ui->RadioSocks5->setChecked(true);
    }

    libJackSMS::LanguageManager *lm=libJackSMS::LanguageManager::getIstance();
    while(lm->nextLanguage()){
        QString lan=lm->currentLang();
        m_ui->languageComboBox->addItem(lan);
    }
    iter=opt.find("language");
    if (iter!=opt.end()){
        int index=m_ui->languageComboBox->findText(iter.value());
        m_ui->languageComboBox->setCurrentIndex(index);
    }
}

OpzioniDialog::~OpzioniDialog()
{
    delete m_ui;
}
void OpzioniDialog::translateGui(){
    libJackSMS::LanguageManager *lm=libJackSMS::LanguageManager::getIstance();
    if(lm->setLanguage("italiano")){

    }
}
void OpzioniDialog::changeEvent(QEvent *e)
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




void OpzioniDialog::on_pushButton_clicked()
{
    this->close();
}



void OpzioniDialog::on_listWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem*)
{
  QString ss=current->text();

    if (ss.contains("Generali"))
        m_ui->stackedWidget->setCurrentIndex(0);
    else if (ss.contains("Rete"))
        m_ui->stackedWidget->setCurrentIndex(1);
    else if (ss.contains("JackSMS Messenger"))
        m_ui->stackedWidget->setCurrentIndex(2);
    else if (ss.contains("Aspetto"))
        m_ui->stackedWidget->setCurrentIndex(3);
    else if (ss.contains("Captcha"))
        m_ui->stackedWidget->setCurrentIndex(4);
    else if (ss.contains("Lingua"))
        m_ui->stackedWidget->setCurrentIndex(5);

}

void OpzioniDialog::on_pushButton_2_clicked()
{
    bool saveGlobal=false;
    if (("yes"==opt["use-proxy"])!=m_ui->CheckUsaProxy->isChecked()){
            opt["use-proxy"]=(m_ui->CheckUsaProxy->isChecked())?"yes":"no";
            saveGlobal=true;

    }

    if (("yes"==opt["use-proxy-auth"])!=m_ui->usaAutenticazione->isChecked()){
            opt["use-proxy-auth"]=(m_ui->usaAutenticazione->isChecked())?"yes":"no";
            saveGlobal=true;

    }

    if (opt["proxy-port"]!=m_ui->TextPort->text()){
        opt["proxy-port"]=m_ui->TextPort->text();
        saveGlobal=true;

    }
    if (opt["proxy-username"]!=m_ui->proxyUsername->text()){
        opt["proxy-username"]=m_ui->proxyUsername->text();
        saveGlobal=true;

    }
    if (opt["proxy-password"]!=m_ui->proxyPassword->text()){
        opt["proxy-password"]=m_ui->proxyPassword->text();
        saveGlobal=true;

    }
    if (opt["proxy"]!=m_ui->TextServer->text()){
        opt["proxy"]=m_ui->TextServer->text();
        saveGlobal=true;

    }


    if (m_ui->RadioHttp->isChecked()){
        if (opt["proxy-type"]!="http"){
            opt["proxy-type"]="http";
            saveGlobal=true;

        }
    }else if (m_ui->RadioSocks5->isChecked()){
       if (opt["proxy-type"]!="socks5"){
            opt["proxy-type"]="socks5";
            saveGlobal=true;

        }
    }

    libJackSMS::dataTypes::optionsType::const_iterator iter=opt.find("language");
    if (iter==opt.end()){

        opt["language"]=m_ui->languageComboBox->currentText();
        this->translateGui();
        emit translate();
        saveGlobal=true;

    }else{
        if (iter.value()!=m_ui->languageComboBox->currentText()){
            opt["language"]=m_ui->languageComboBox->currentText();
            this->translateGui();
            emit translate();
            saveGlobal=true;
        }

    }
    if(saveGlobal){
        libJackSMS::dataTypes::optionsType opt2;
        opt2["language"]=opt["language"];
        opt2["proxy-port"]=opt["proxy-port"];
        opt2["proxy-username"]= opt["proxy-username"];
        opt2["proxy-password"]=opt["proxy-password"];
        opt2["proxy"]=opt["proxy"];
        opt2["proxy-type"]=opt["proxy-type"];
        opt2["use-proxy"]=opt["use-proxy"];
        opt2["use-proxy-auth"]=opt["use-proxy-auth"];
        opt2["save-passwd"]=opt["save-passwd"];

        libJackSMS::localApi::optionManager op("",opt2);

        try{
            op.save();
        }catch(libJackSMS::exceptionXmlError e){
            QMessageBox::critical(this,"JackSMS","Errore durante il salvataggio delle opzioni\n"+QString(e.what()));
        }catch(...){
            QMessageBox::critical(this,"JackSMS","Errore sconosciuto");
        }
    }

    if (loggedIn){
        /*if (("yes"==opt["set-bgcolor"])!=m_ui->settaColoreBackground->isChecked()){
            opt["set-bgcolor"]=(m_ui->settaColoreBackground->isChecked())?"yes":"no";
        }*/
        if (("yes"==opt["set-account"])!=m_ui->checkAccountDefault->isChecked()){
            opt["set-account"]=(m_ui->checkAccountDefault->isChecked())?"yes":"no";

        }

        if (("yes"==opt["save-local"])!=m_ui->checkSalvalocale->isChecked()){
            opt["save-local"]=(m_ui->checkSalvalocale->isChecked())?"yes":"no";

        }

        if (("no"!=opt["receive-im"])&& (!m_ui->CheckAbilitaIM->isChecked())){

            opt["receive-im"]="no";
            //emit deactivateImTimer();
        }else if (("no"==opt["receive-im"])&& (m_ui->CheckAbilitaIM->isChecked())){

            opt["receive-im"]="yes";
            //emit activateImTimer();
        }

         if (opt["textsize"]!=m_ui->ComboDimensione->currentText()){
            opt["textsize"]=m_ui->ComboDimensione->currentText();
            TextSms.setFont(QFont(TextSms.font().family(),m_ui->ComboDimensione->currentText().toInt(NULL,10),TextSms.font().weight(),false));

        }
         if (opt["captcha-zoom"]!=m_ui->comboZoomCaptcha->currentText()){
             opt["captcha-zoom"]=m_ui->comboZoomCaptcha->currentText();

         }
        if(("yes"==opt["use-captcha"]) != m_ui->checkUseCaptcha->isChecked()){
             opt["use-captcha"] = (m_ui->checkUseCaptcha->isChecked())?"yes":"no";

         }
       // QMessageBox::information(this,"asd",userDirectory);
        libJackSMS::localApi::optionManager op(userDirectory,opt);

        try{
            op.save();
        }catch(libJackSMS::exceptionXmlError e){
            QMessageBox::critical(this,"JackSMS","Errore durante il salvataggio delle opzioni\n"+QString(e.what()));
        }catch(...){
            QMessageBox::critical(this,"JackSMS","Errore sconosciuto");
        }
    }


}


void OpzioniDialog::on_pushButton_3_clicked()
{
    this->on_pushButton_2_clicked();
    this->close();
}

void OpzioniDialog::on_listWidget_itemClicked(QListWidgetItem* item)
{

}
