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

#include "editaccountdialog.h"
#include "ui_editaccountdialog.h"
#include <QLineEdit>
#include <QLabel>
#include <libjacksms/libJackSMS.h>
#include <QMessageBox>

editAccountDialog::editAccountDialog(libJackSMS::dataTypes::configuredServicesType &_acc, libJackSMS::dataTypes::servicesType &_services, QString _id, QString _current_login_id, QWidget *parent) :
    QDialog(parent),
    accounts(_acc),
    services(_services),
    id(_id),
    current_login_id(_current_login_id),
    ui(new Ui::editAccountDialog)
{
    ui->setupUi(this);

    spinner = new QMovie(":/resource/loading-spinner.gif",QByteArray("gif"),this);
    spinner->setScaledSize(QSize(16,16));
    spinner->start();
    ui->labelSpin->setMovie(spinner);
    ui->labelSpin->hide();

    libJackSMS::dataTypes::configuredServicesType::const_iterator iterator = accounts.find(id);
    libJackSMS::dataTypes::servicesType::iterator iterator_serv = services.find(iterator->getService());
    if (iterator_serv != services.end()) {
        currentAccount = accounts[id];
        QString nomeServizio = currentAccount.getName();

        for (int i = ui->serviceDataForms->rowCount() - 1; i >= 0; i--) {
            QWidget* w1 = ui->serviceDataForms->itemAt(i, QFormLayout::LabelRole)->widget();
            QWidget* w2 = ui->serviceDataForms->itemAt(i, QFormLayout::FieldRole)->widget();

            ui->serviceDataForms->removeWidget(w1);
            ui->serviceDataForms->removeWidget(w2);

           delete w1;
           delete w2;
        }

        ui->serviceDataForms->addRow("Nome", new QLineEdit(nomeServizio));
        QLineEdit* linePtr;
        while (iterator_serv.value().nextVar()) {
            libJackSMS::dataTypes::variabileServizio i = iterator_serv.value().currentVar();
            linePtr = new QLineEdit();
            linePtr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

            if(i.getName() == "password" || i.getName() == "passwd" || i.getName() == "psw")
                linePtr->setEchoMode(QLineEdit::Password);

            linePtr->setToolTip(i.getDescription());
            linePtr->setText(iterator->getData(i.getName()));

            ui->serviceDataForms->addRow(i.getName(), linePtr);

        }
    }
}

editAccountDialog::~editAccountDialog()
{
    delete ui;
}

void editAccountDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void editAccountDialog::on_buttonAnnulla_clicked()
{
    close();
}
void editAccountDialog::updateOk(libJackSMS::dataTypes::configuredAccount acc){
    accounts[acc.getId()]=acc;
    emit rewriteAccunts();
    close();
}
void editAccountDialog::accountNotUpdated(){
    ui->buttonAnnulla->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->labelSpin->hide();
    QMessageBox::critical(this, "Freesmee", "Si e' verificato un errore durante l'aggiornamento dell'account.");
}

void editAccountDialog::on_pushButton_clicked()
{

    QLineEdit *it;
    QString accountName;
    it = dynamic_cast<QLineEdit*>(ui->serviceDataForms->itemAt(0, QFormLayout::FieldRole)->widget());
    accountName=it->text();

    libJackSMS::dataTypes::configuredServicesType::const_iterator i=accounts.begin();
    libJackSMS::dataTypes::configuredServicesType::const_iterator i_end=accounts.end();

    bool save=true;
    while(i!=i_end){

        if (i.value().getId()!=currentAccount.getId() && i.value().getName()==accountName){
            save=false;
            break;
        }
        ++i;
    }
    if (!save){
        QMessageBox::information(this, "Freesmee", "Esiste un'altro servizio con questo nome.\nModificalo e risalva il servizio.");
    }else{

        currentAccount.setName(accountName);

        for(int i = ui->serviceDataForms->rowCount() - 1; i >= 0; i--){
            QString key =dynamic_cast<QLabel*>(ui->serviceDataForms->itemAt(i, QFormLayout::LabelRole)->widget())->text();
            QString value =dynamic_cast<QLineEdit*>(ui->serviceDataForms->itemAt(i, QFormLayout::FieldRole)->widget())->text();
            if (value.toStdString().empty()){
                QMessageBox::information(this, "Freesmee", "Il campo "+key+" e' vuoto");
                return;
            }
            currentAccount.setData(key,value);
        }
        ui->buttonAnnulla->setEnabled(false);
        ui->pushButton->setEnabled(false);
        ui->labelSpin->show();

        saver = new libJackSMS::serverApi::accountManager(current_login_id);
        connect(saver,SIGNAL(accountUpdated(libJackSMS::dataTypes::configuredAccount)),this,SLOT(updateOk(libJackSMS::dataTypes::configuredAccount)));
        connect(saver,SIGNAL(accountNotUpdated()),this,SLOT(accountNotUpdated()));
        saver->updateAccount(currentAccount,services[currentAccount.getService()]);

    }
}
