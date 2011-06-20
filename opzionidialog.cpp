/*
    Copyright (C) <2011>

    <enrico bacis> <enrico.bacis@gmail.com>
    <ivan vaccari> <grisson@jacksms.it>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    You can't modify the adv system, to cheat it.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

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
    // disabilito momentaneamente la scheda "lingua"
    m_ui->listOpzioni->item(4)->~QListWidgetItem();

    if (!loggedIn) {
        //m_ui->listOpzioni->item(4)->~QListWidgetItem();
        m_ui->listOpzioni->item(3)->~QListWidgetItem();
        m_ui->listOpzioni->item(2)->~QListWidgetItem();
        m_ui->listOpzioni->item(0)->~QListWidgetItem();
        m_ui->stackedWidget->setCurrentIndex(1);
    } else {
        m_ui->stackedWidget->setCurrentIndex(0);

        // rimuovo momentaneamente il widget. in versioni superiori verrà reintrodotto
        m_ui->listOpzioni->item(3)->~QListWidgetItem();

        setCheckboxStatusFromYesNoOption(globopt, m_ui->opzAutoLogin, "auto-login");

        setCheckboxStatusFromYesNoOption(opt, m_ui->checkAccountDefault, "set-account");
        setCheckboxStatusFromYesNoOption(opt, m_ui->checkSalvalocale, "save-local");
        setCheckboxStatusFromYesNoOption(opt, m_ui->checkShowAdv, "show-adv");
        setCheckboxStatusFromYesNoOption(opt, m_ui->successSmsPopup, "successfull-send-popup");
        setCheckboxStatusFromYesNoOption(opt, m_ui->captchaPopup, "display-captcha-popup");
        setCheckboxStatusFromYesNoOption(opt, m_ui->errorSmsPopup, "error-send-popup");
        setCheckboxStatusFromYesNoOption(opt, m_ui->opzSvuotaInvioCorretto, "svuota-invio-corretto");
        setCheckboxStatusFromYesNoOption(opt, m_ui->nonSalvaCookies, "dont-cookies");
        setCheckboxStatusFromYesNoOption(opt, m_ui->hideServiceUpdate, "hide-service-update");
        setCheckboxStatusFromYesNoOption(opt, m_ui->suonoJMS, "suono-free");
        setCheckboxStatusFromYesNoOption(opt, m_ui->showPopupJmsStatus, "show-popup-jms-status");
        setCheckboxStatusFromYesNoOption(opt, m_ui->showPopupNewJms, "show-popup-new-free");

        // rimuovo intanto che non sono ancora implementate
        /*setCheckboxStatusFromYesNoOption(opt, m_ui->checkUseCaptcha, "use-captcha");

        iter = opt.find("captcha-zoom");
        if (iter != opt.end()) {
            int index = m_ui->comboZoomCaptcha->findText(opt["captcha-zoom"]);
            m_ui->comboZoomCaptcha->setCurrentIndex((index == -1) ? 0 : index);
        }*/
    }

    setCheckboxStatusFromYesNoOption(globopt, m_ui->CheckUsaProxy, "use-proxy");
    setCheckboxStatusFromYesNoOption(globopt, m_ui->usaAutenticazione, "use-proxy-auth");
    setTextFromOption(globopt, m_ui->TextPort, "proxy-port");
    setTextFromOption(globopt, m_ui->proxyUsername, "proxy-username");
    setTextFromOption(globopt, m_ui->proxyPassword, "proxy-password");
    setTextFromOption(globopt, m_ui->TextServer, "proxy");

    libJackSMS::dataTypes::optionsType::const_iterator iter;
    iter = globopt.find("proxy-type");
    if (iter != globopt.end()) {
        if (globopt["proxy-type"] == "http")
            m_ui->RadioHttp->setChecked(true);
        else if (globopt["proxy-type"] == "socks5")
            m_ui->RadioSocks5->setChecked(true);
    }

    // rimuovo intanto che non sono ancora implementate
    /*libJackSMS::LanguageManager *lm = libJackSMS::LanguageManager::getIstance();
    while(lm->nextLanguage()) {
        QString lan = lm->currentLang();
        m_ui->languageComboBox->addItem(lan);
    }

    iter = opt.find("language");
    if (iter != opt.end()) {
        int index = m_ui->languageComboBox->findText(iter.value());
        m_ui->languageComboBox->setCurrentIndex(index);
    }*/
}

OpzioniDialog::~OpzioniDialog()
{
    delete m_ui;
}

void OpzioniDialog::translateGui(){
    libJackSMS::LanguageManager *lm = libJackSMS::LanguageManager::getIstance();
    if (lm->setLanguage("italiano")) {}
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

void OpzioniDialog::on_annullaButton_clicked()
{
    close();
}

void OpzioniDialog::on_listOpzioni_currentItemChanged(QListWidgetItem* current, QListWidgetItem*)
{
    QString ss = current->text();

    if (ss.contains("Generali"))
        m_ui->stackedWidget->setCurrentIndex(0);
    else if (ss.contains("Rete"))
        m_ui->stackedWidget->setCurrentIndex(1);
    else if (ss.contains("Free+"))
        m_ui->stackedWidget->setCurrentIndex(2);
    else if (ss.contains("Aspetto"))
        m_ui->stackedWidget->setCurrentIndex(3);
    else if (ss.contains("Captcha"))
        m_ui->stackedWidget->setCurrentIndex(4);
    else if (ss.contains("Lingua"))
        m_ui->stackedWidget->setCurrentIndex(5);
}

void OpzioniDialog::on_applicaButton_clicked() {

    globopt["use-proxy"] = m_ui->CheckUsaProxy->isChecked() ? "yes" : "no";
    globopt["use-proxy-auth"] = m_ui->usaAutenticazione->isChecked() ? "yes" : "no";
    globopt["proxy-port"] = m_ui->TextPort->text();
    globopt["proxy-username"] = m_ui->proxyUsername->text();
    globopt["proxy-password"] = m_ui->proxyPassword->text();
    globopt["proxy"] = m_ui->TextServer->text();

    if (m_ui->RadioHttp->isChecked())
        globopt["proxy-type"] = "http";
    else if (m_ui->RadioSocks5->isChecked())
        globopt["proxy-type"] = "socks5";

    globopt["auto-login"] = m_ui->opzAutoLogin->isChecked() ? "yes" : "no";

    // rimuovo intanto che non sono ancora implementate
    /*
    libJackSMS::dataTypes::optionsType::const_iterator iter=opt.find("language");
    if (iter == opt.end()) {
        globopt["language"] = m_ui->languageComboBox->currentText();
        translateGui();
        emit translate();
    } else {
        if (iter.value() != m_ui->languageComboBox->currentText()) {
            globopt["language"] = m_ui->languageComboBox->currentText();
            translateGui();
            emit translate();
        }
    }
    */

    libJackSMS::localApi::optionManager op("", globopt);

    try {
        op.save();
    } catch (libJackSMS::exceptionXmlError e) {
        QMessageBox::critical(this, "Freesmee", "Errore durante il salvataggio delle opzioni\n" + QString(e.what()));
    } catch (...) {
        QMessageBox::critical(this, "Freesmee", "Errore sconosciuto");
    }

    if (loggedIn) {

        // se autologin è su si allora dobbiamo anche salvare la password nelle opzioni
        if (globopt["auto-login"] == "yes")
            opt["password"] = pass;

        opt["set-account"] = m_ui->checkAccountDefault->isChecked() ? "yes" : "no";
        opt["show-adv"] = m_ui->checkShowAdv->isChecked() ? "yes" : "no";
        opt["save-local"] = m_ui->checkSalvalocale->isChecked() ? "yes" : "no";
        opt["successfull-send-popup"] = m_ui->successSmsPopup->isChecked() ? "yes" : "no";
        opt["display-captcha-popup"] = m_ui->captchaPopup->isChecked() ? "yes" : "no";
        opt["error-send-popup"] = m_ui->errorSmsPopup->isChecked() ? "yes" : "no";
        opt["svuota-invio-corretto"] = m_ui->opzSvuotaInvioCorretto->isChecked() ? "yes" : "no";
        opt["dont-cookies"] = m_ui->nonSalvaCookies->isChecked() ? "yes" : "no";
        opt["hide-service-update"] = m_ui->hideServiceUpdate->isChecked() ? "yes" : "no";
        opt["suono-free"] = m_ui->suonoJMS->isChecked() ? "yes" : "no";
        opt["show-popup-jms-status"] = m_ui->showPopupJmsStatus->isChecked() ? "yes" : "no";
        opt["show-popup-new-free"] = m_ui->showPopupNewJms->isChecked() ? "yes" : "no";

        // rimuovo intanto che non sono ancora implementate
        //opt["captcha-zoom"] = m_ui->comboZoomCaptcha->currentText();
        //opt["use-captcha"] = (m_ui->checkUseCaptcha->isChecked()) ? "yes" : "no";

        libJackSMS::localApi::optionManager op2(userDirectory, opt);

        try {
            op2.save();
        } catch (libJackSMS::exceptionXmlError e) {
            QMessageBox::critical(this, "Freesmee", "Errore durante il salvataggio delle opzioni\n"+QString(e.what()));
        } catch (...) {
            QMessageBox::critical(this, "Freesmee", "Errore sconosciuto");
        }
    }
}

void OpzioniDialog::on_okButton_clicked()
{
    on_applicaButton_clicked();
    close();
}

void OpzioniDialog::setCheckboxStatusFromYesNoOption(libJackSMS::dataTypes::optionsType &currentWorkingOpt, QCheckBox *checkbox, QString optionName)
{
    if (currentWorkingOpt[optionName] == "yes")
        checkbox->setChecked(true);
    else if (currentWorkingOpt[optionName] == "no")
        checkbox->setChecked(false);
}

void OpzioniDialog::setTextFromOption(libJackSMS::dataTypes::optionsType &currentWorkingOpt, QLineEdit *lineedit, QString optionName)
{
    libJackSMS::dataTypes::optionsType::const_iterator iter = currentWorkingOpt.find(optionName);
    if (iter != currentWorkingOpt.end())
        lineedit->setText(iter.value());
}
