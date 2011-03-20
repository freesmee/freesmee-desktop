#include "opzionidialog.h"
#include "ui_opzionidialog.h"
#include <QListWidgetItem>
#include <QTextEdit>
#include <QMessageBox>
#include "Configuration.h"
#include "JackUtils.h"
#include <QTimer>

OpzioniDialog::OpzioniDialog(libJackSMS::dataTypes::optionsType &_opt, libJackSMS::dataTypes::optionsType &_globopt, QString _userDirectory, QWidget *parent, const bool _loggedIn, QString _pass) :
    QDialog(parent),
    opt(_opt),
    globopt(_globopt),
    userDirectory(_userDirectory),
    loggedIn(_loggedIn),
    pass(_pass),
    m_ui(new Ui::OpzioniDialog)
{
    m_ui->setupUi(this);
    libJackSMS::dataTypes::optionsType::const_iterator iter;

    // !!!!!!!!!!!!!!!!! disabilito momentaneamente la scheda "lingua"
    m_ui->listWidget->item(4)->~QListWidgetItem();
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    if (!loggedIn) {
        //m_ui->listWidget->item(4)->~QListWidgetItem();
        m_ui->listWidget->item(3)->~QListWidgetItem();
        m_ui->listWidget->item(2)->~QListWidgetItem();
        m_ui->listWidget->item(0)->~QListWidgetItem();

        m_ui->stackedWidget->setCurrentIndex(1);
    } else {
        /*****ATTENZIONE: rimuovo momentaneamente il widget. in versioni superiori verrà reintrodotto*/
        m_ui->listWidget->item(3)->~QListWidgetItem();
        /******/

        iter = opt.find("set-account");
        if (iter != opt.end()) {
            if ("no" == iter.value()) {
                m_ui->checkAccountDefault->setChecked(false);
            } else {
                m_ui->checkAccountDefault->setChecked(true);
            }
        } else {
            m_ui->checkAccountDefault->setChecked(true);
        }

        iter = globopt.find("auto-login");
        if (iter != globopt.end())
            if (iter.value() == "yes")
                m_ui->opzAutoLogin->setChecked(true);

        iter = opt.find("save-local");
        if (iter != opt.end()) {
            if ("no" == iter.value()) {
                m_ui->checkSalvalocale->setChecked(false);
            } else {
                m_ui->checkSalvalocale->setChecked(true);
            }
        } else {
            m_ui->checkSalvalocale->setChecked(true);
        }

        iter=opt.find("opz-svuota-invio-corretto");
        if (iter!=opt.end())
            if ("yes"==iter.value())
                m_ui->opzSvuotaInvioCorretto->setChecked(true);

        iter=opt.find("dont-cookies");
        if (iter!=opt.end())
            if ("yes"==iter.value())
                m_ui->nonSalvaCookies->setChecked(true);

        iter=opt.find("hide-service-update");
        if (iter!=opt.end())
            if ("yes"==iter.value())
                m_ui->hideServiceUpdate->setChecked(true);

        iter=opt.find("suono-jms");
        if (iter!=opt.end())
            if ("yes"==iter.value())
                m_ui->suonoJMS->setChecked(true);

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

        iter=opt.find("captcha-zoom");
        if (iter!=opt.end()){
            int index=m_ui->comboZoomCaptcha->findText(opt["captcha-zoom"]);

            m_ui->comboZoomCaptcha->setCurrentIndex((index==-1)?0:index);
        }

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

    iter=opt.find("successfull-send-popup");
    if (iter!=opt.end())
        if (iter.value()=="yes")
            m_ui->successSmsPopup->setChecked(true);

    iter=opt.find("display-captcha-popup");
    if (iter!=opt.end())
        if (iter.value()=="yes")
            m_ui->captchaPopup->setChecked(true);

    iter = opt.find("error-send-popup");
    if (iter != opt.end()) {
        if (iter.value() == "no") {
            m_ui->errorSmsPopup->setChecked(false);
        } else {
            m_ui->errorSmsPopup->setChecked(true);
        }
    } else {
        m_ui->errorSmsPopup->setChecked(true);
    }

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
    close();
}

void OpzioniDialog::on_listWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem*)
{
  QString ss = current->text();

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
    bool saveGlobal = false;

    if (("yes" == opt["use-proxy"]) != m_ui->CheckUsaProxy->isChecked()) {
        opt["use-proxy"] = (m_ui->CheckUsaProxy->isChecked()) ? "yes" : "no";
        saveGlobal = true;
    }

    if (("yes" == opt["use-proxy-auth"]) != m_ui->usaAutenticazione->isChecked()) {
        opt["use-proxy-auth"] = (m_ui->usaAutenticazione->isChecked()) ? "yes" : "no";
        saveGlobal = true;
    }

    if (opt["proxy-port"] != m_ui->TextPort->text()) {
        opt["proxy-port"] = m_ui->TextPort->text();
        saveGlobal = true;
    }

    if (opt["proxy-username"] != m_ui->proxyUsername->text()) {
        opt["proxy-username"] = m_ui->proxyUsername->text();
        saveGlobal = true;
    }

    if (opt["proxy-password"] != m_ui->proxyPassword->text()) {
        opt["proxy-password"] = m_ui->proxyPassword->text();
        saveGlobal = true;
    }

    if (opt["proxy"] != m_ui->TextServer->text()) {
        opt["proxy"] = m_ui->TextServer->text();
        saveGlobal = true;
    }

    if (m_ui->RadioHttp->isChecked()) {
        if (opt["proxy-type"] != "http") {
            opt["proxy-type"] = "http";
            saveGlobal = true;
        }
    } else if (m_ui->RadioSocks5->isChecked()) {
        if (opt["proxy-type"] != "socks5"){
            opt["proxy-type"] = "socks5";
            saveGlobal = true;
        }
    }

    if (("yes" == globopt["auto-login"]) != m_ui->opzAutoLogin->isChecked()) {
        globopt["auto-login"] = (m_ui->opzAutoLogin->isChecked()) ? "yes" : "no";
        saveGlobal = true;

        // se è cambiato ed è stato messo su si allora dobbiamo anche salvare la password nelle opzioni e il default-user nelle opzioni globali
        if (globopt["auto-login"] == "yes") {
            opt["password"] = pass;
        }
    }

    libJackSMS::dataTypes::optionsType::const_iterator iter=opt.find("language");
    if (iter == opt.end()) {

        opt["language"] = m_ui->languageComboBox->currentText();
        translateGui();
        emit translate();
        saveGlobal = true;

    } else {
        if (iter.value() != m_ui->languageComboBox->currentText()) {
            opt["language"] = m_ui->languageComboBox->currentText();
            translateGui();
            emit translate();
            saveGlobal = true;
        }
    }

    if(saveGlobal) {
        libJackSMS::dataTypes::optionsType opt2;
        opt2["language"] = opt["language"];
        opt2["proxy-port"] = opt["proxy-port"];
        opt2["proxy-username"] = opt["proxy-username"];
        opt2["proxy-password"] = opt["proxy-password"];
        opt2["proxy"] = opt["proxy"];
        opt2["proxy-type"] = opt["proxy-type"];
        opt2["use-proxy"] = opt["use-proxy"];
        opt2["use-proxy-auth"] = opt["use-proxy-auth"];
        opt2["auto-login"] = globopt["auto-login"];

        libJackSMS::localApi::optionManager op("", opt2);

        try {
            op.save();
        } catch(libJackSMS::exceptionXmlError e) {
            QMessageBox::critical(this, "JackSMS", "Errore durante il salvataggio delle opzioni\n" + QString(e.what()));
        } catch(...) {
            QMessageBox::critical(this, "JackSMS", "Errore sconosciuto");
        }
    }

    if (loggedIn) {

        opt["set-account"] = (m_ui->checkAccountDefault->isChecked()) ? "yes" : "no";

        opt["save-local"] = (m_ui->checkSalvalocale->isChecked()) ? "yes" : "no";

        if (("yes"==opt["successfull-send-popup"])!=m_ui->successSmsPopup->isChecked())
            opt["successfull-send-popup"]=(m_ui->successSmsPopup->isChecked())?"yes":"no";

        if (("yes"==opt["display-captcha-popup"])!=m_ui->captchaPopup->isChecked())
            opt["display-captcha-popup"]=(m_ui->captchaPopup->isChecked())?"yes":"no";

        opt["error-send-popup"] = (m_ui->errorSmsPopup->isChecked()) ? "yes" : "no";

        if (("yes"==opt["dont-cookies"])!=m_ui->nonSalvaCookies->isChecked())
            opt["dont-cookies"]=(m_ui->nonSalvaCookies->isChecked())?"yes":"no";

        if (("yes"==opt["hide-service-update"])!=m_ui->hideServiceUpdate->isChecked())
            opt["hide-service-update"]=(m_ui->hideServiceUpdate->isChecked())?"yes":"no";

        if (("yes"==opt["suono-jms"])!=m_ui->suonoJMS->isChecked())
            opt["suono-jms"]=(m_ui->suonoJMS->isChecked())?"yes":"no";

        if (("yes"==opt["opz-svuota-invio-corretto"])!=m_ui->opzSvuotaInvioCorretto->isChecked())
            opt["opz-svuota-invio-corretto"]=(m_ui->opzSvuotaInvioCorretto->isChecked())?"yes":"no";

        if (("no"!=opt["receive-im"])&& (!m_ui->CheckAbilitaIM->isChecked()))
            opt["receive-im"]="no";
        else if (("no"==opt["receive-im"])&& (m_ui->CheckAbilitaIM->isChecked()))
            opt["receive-im"]="yes";

        if (opt["captcha-zoom"]!=m_ui->comboZoomCaptcha->currentText())
            opt["captcha-zoom"]=m_ui->comboZoomCaptcha->currentText();

        if(("yes"==opt["use-captcha"]) != m_ui->checkUseCaptcha->isChecked())
            opt["use-captcha"] = (m_ui->checkUseCaptcha->isChecked())?"yes":"no";

        libJackSMS::localApi::optionManager op(userDirectory,opt);

        try {
            op.save();
        } catch(libJackSMS::exceptionXmlError e) {
            QMessageBox::critical(this,"JackSMS","Errore durante il salvataggio delle opzioni\n"+QString(e.what()));
        } catch(...) {
            QMessageBox::critical(this,"JackSMS","Errore sconosciuto");
        }

    }
}

void OpzioniDialog::on_pushButton_3_clicked()
{
    on_pushButton_2_clicked();
    close();
}
