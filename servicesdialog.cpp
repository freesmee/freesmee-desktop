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

#include "Configuration.h"
#include "servicesdialog.h"
#include "ui_servicesdialog.h"
#include "Types.h"
#include <QIcon>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QProcess>
#include <QString>

#include "mainjacksms.h"
#include "avvisoaccentidialog.h"

ServicesDialog::ServicesDialog(QWidget *parent,MainJackSMS * _padre,libJackSMS::dataTypes::servicesType &_ElencoServizi,libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,const libJackSMS::dataTypes::optionsType &_opzioni) :
    QDialog(parent),
    padre(_padre),
    m_ui(new Ui::ServicesDialog),
    ElencoServizi(_ElencoServizi),
    ElencoServiziConfigurati(_ElencoServiziConfigurati),

    opzioni(_opzioni)
{
    m_ui->setupUi(this);

    for (libJackSMS::dataTypes::servicesType::const_iterator i = ElencoServizi.begin(); i != ElencoServizi.end(); ++i) {

        if (i.value().getId() == "1")
            continue;

        QListWidgetItem *it = new QListWidgetItem();
        it->setText(i.value().getName());
        it->setIcon(i.value().getIcon());
        it->setWhatsThis(i.key());
        m_ui->ListServizi->addItem(it);
    }

    m_ui->ListServizi->sortItems(Qt::AscendingOrder);

    spinner=new QMovie(":/resource/loading-spinner.gif",QByteArray("gif"),this);
    spinner->setScaledSize(QSize(16,16));
    spinner->start();
    m_ui->labelSpinAddAccount->setMovie(spinner);
    m_ui->labelSpinAddAccount->hide();
}

void ServicesDialog::addAccountKo() {
    m_ui->Salva->show();
    m_ui->labelSpinAddAccount->hide();
    QMessageBox::critical(this, "Freesmee", "Si è verificato un errore durante il salvataggio dell'account.");
}

void ServicesDialog::addAccountOk(QString id) {
    newAcc.setId(id);
    ElencoServiziConfigurati.insert(id, newAcc);
    m_ui->Salva->show();
    m_ui->labelSpinAddAccount->hide();
    emit rewriteAccounts();
    close();
}

ServicesDialog::~ServicesDialog()
{
    delete m_ui;
}

void ServicesDialog::changeEvent(QEvent *e)
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

void ServicesDialog::on_ListServizi_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    QString id = current->whatsThis();
    currentId = id;
    libJackSMS::dataTypes::servicesType::iterator iterator_serv = ElencoServizi.find(id);
    if (iterator_serv != ElencoServizi.end()) {
        QString nomeServizio = iterator_serv.value().getName();
        //QString info = QString::fromUtf8(iterator_serv->second.getDescription().c_str(),iterator_serv->second.getDescription().length());
        QString info = iterator_serv.value().getDescription();

        QString link("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\"> <p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"http://www.freesmee.com/servizio-sms/" + nomeServizio + ".html\"><img src=\":/resource/external-link-ltr-icon.png\" /><span style=\" font-size:14pt; text-decoration: none; color:#000000;\">" + nomeServizio + "</span></a></p></body></html>");
        info = info.replace("\\n", "\n");
        m_ui->DescrizioneServizio->setText(info);
        m_ui->labelNomeServizio->setText(link);
        m_ui->smsSingolo->setText("Lunghezza sms singolo: " + iterator_serv.value().getSingleLength());
        m_ui->smsLungo->setText("Lunghezza sms totale: " + iterator_serv.value().getMaxLength());

        for(int i = m_ui->serviceDataForms->rowCount() - 1; i >= 0; i--) {
            QWidget* w1 = m_ui->serviceDataForms->itemAt(i, QFormLayout::LabelRole)->widget();
            QWidget* w2 = m_ui->serviceDataForms->itemAt(i, QFormLayout::FieldRole)->widget();

            m_ui->serviceDataForms->removeWidget(w1);
            m_ui->serviceDataForms->removeWidget(w2);

           delete w1;
           delete w2;
        }

        delete m_ui->serviceDataForms;
        m_ui->serviceDataForms = new QFormLayout(m_ui->groupBox);
        m_ui->serviceDataForms->addRow("nome", new QLineEdit(nomeServizio));
        QLineEdit* linePtr;
        while(iterator_serv.value().nextVar()){
            libJackSMS::dataTypes::variabileServizio i=iterator_serv.value().currentVar();
            linePtr = new QLineEdit();
            if(i.getName() == "password" || i.getName() == "passwd" || i.getName() == "psw")
                linePtr->setEchoMode(QLineEdit::Password);
            linePtr->setToolTip(i.getDescription());
            if (!i.getDefaultValue().isEmpty())
                    linePtr->setText(i.getDefaultValue());

            m_ui->serviceDataForms->addRow(i.getName(), linePtr);
        }
    }
}

void ServicesDialog::on_Salva_clicked()
{
    newAcc = libJackSMS::dataTypes::configuredAccount();

    QLineEdit *it;
    QString accountName;
    it = dynamic_cast<QLineEdit*>(m_ui->serviceDataForms->itemAt(0, QFormLayout::FieldRole)->widget());
    accountName = it->text();

    libJackSMS::dataTypes::configuredServicesType::const_iterator i=ElencoServiziConfigurati.begin();
    libJackSMS::dataTypes::configuredServicesType::const_iterator i_end=ElencoServiziConfigurati.end();

    bool save = true;
    while (i != i_end) {

        if (i.value().getName() == accountName) {
            save = false;
            break;
        }
        ++i;
    }

    if (!save) {
        QMessageBox::information(this, "Freesmee", "Esiste un'altro servizio con questo nome.\nModificalo e risalva il servizio.");
    } else {

        newAcc.setName(accountName);
        newAcc.setService(currentId);
        for(int i = m_ui->serviceDataForms->rowCount() - 1; i >= 0; i--){
            QString key =dynamic_cast<QLabel*>(m_ui->serviceDataForms->itemAt(i, QFormLayout::LabelRole)->widget())->text();
            QString value =dynamic_cast<QLineEdit*>(m_ui->serviceDataForms->itemAt(i, QFormLayout::FieldRole)->widget())->text();
            if (value.toStdString().empty()){
                QMessageBox::information(this, "Freesmee", "Il campo "+key+" e' vuoto");
                return;
            }
            newAcc.setData(key,value);
        }

        m_ui->Salva->hide();
        m_ui->labelSpinAddAccount->show();

        saver = new libJackSMS::serverApi::accountManager(padre->current_login_id, opzioni);
        connect(saver, SIGNAL(accountSaved(QString)), this, SLOT(addAccountOk(QString)));
        connect(saver, SIGNAL(accountNotSaved()), this, SLOT(addAccountKo()));
        saver->addNewAccount(ElencoServizi[currentId], newAcc);

    }
}
