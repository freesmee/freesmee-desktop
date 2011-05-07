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

#include "editcontattodialog.h"
#include "ui_editcontattodialog.h"
#include <QProcess>
#include <Configuration.h>
#include <QMessageBox>

editcontattodialog::editcontattodialog(QWidget *parent, const libJackSMS::dataTypes::servicesType &_ElencoServizi, const libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati, libJackSMS::dataTypes::phoneBookType &_Rubrica, QString contactId, const libJackSMS::dataTypes::optionsType _Opzioni, QString _current_login_id) :
        QDialog(parent),
        m_ui(new Ui::editcontattodialog),
        ElencoServizi(_ElencoServizi),
        ElencoServiziConfigurati(_ElencoServiziConfigurati),
        Rubrica(_Rubrica),
        Opzioni(_Opzioni),
        id(contactId),
        current_login_id(_current_login_id)
{
    m_ui->setupUi(this);
    spinner = new QMovie(":/resource/loading-spinner.gif", QByteArray("gif"), this);
    spinner->setScaledSize(QSize(16, 16));
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
            if (i.value().getId() != "1") {
                m_ui->comboaccount->addItem(ElencoServizi[i.value().getService()].getIcon(),i.value().getName());
            }
        }
        m_ui->comboaccount->model()->sort(0);
        m_ui->comboaccount->insertItem(0, ElencoServizi[ElencoServiziConfigurati["1"].getService()].getIcon(), ElencoServiziConfigurati["1"].getName());
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
    close();
}

void editcontattodialog::on_salva_clicked()
{
    QString nome = m_ui->nome->text();
    libJackSMS::dataTypes::phoneNumber num;
    bool numeroValido;
    numeroValido = num.parse( (m_ui->intPref->text().isEmpty() ? "" : m_ui->intPref->text() + ".") + m_ui->pref->text() + "." + m_ui->num->text());

    if (!numeroValido) {
        QMessageBox::critical(this, "Attenzione", "Il numero inserito non è valido, ricontrollalo, oppure se trovi che questo sia un errore segnalalo, grazie.");
        return;
    }

    QString idAccount = "";

    {
        QString nomeAccount = m_ui->comboaccount->currentText();

        for (libJackSMS::dataTypes::configuredServicesType::const_iterator i = ElencoServiziConfigurati.begin(); i != ElencoServiziConfigurati.end(); ++i) {
            if (i.value().getName() == nomeAccount) {
                idAccount = i.value().getId();
                break;
            }
        }
    }

    libJackSMS::dataTypes::contact contatto(nome, num, "", idAccount);
    contatto.setId(id);
    m_ui->salva->setEnabled(false);
    m_ui->annulla->setEnabled(false);
    m_ui->labelSpin->show();

    saver = new libJackSMS::serverApi::contactManager(current_login_id, Opzioni);
    connect(saver, SIGNAL(contactUpdated(libJackSMS::dataTypes::contact)), this, SLOT(salvataggioOk(libJackSMS::dataTypes::contact)));
    connect(saver, SIGNAL(contactNotUpdated()), this, SLOT(salvataggioKo()));
    saver->updateContact(contatto);
}

void editcontattodialog::salvataggioOk(libJackSMS::dataTypes::contact c) {
    Rubrica[c.getId()] = c;
    emit contactEdited(c.getId());
    close();
}

void editcontattodialog::salvataggioKo(){
    m_ui->salva->setEnabled(true);
    m_ui->annulla->setEnabled(true);
    m_ui->labelSpin->hide();
    QMessageBox::critical(this, "Freesmee", "Si e' verificato un errore durante l'aggiornamento del contatto.");
}
