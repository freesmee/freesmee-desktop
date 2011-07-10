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

#include "cambiaaccount.h"
#include "ui_cambiaaccount.h"
#include <libjacksms/libJackSMS.h>

cambiaaccount::cambiaaccount(QWidget *parent, MainJackSMS *_padre, const libJackSMS::dataTypes::servicesType &_ElencoServizi, const libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati, libJackSMS::dataTypes::phoneBookType &_Rubrica, QString _id, int _found) :
    QDialog(parent),
    m_ui(new Ui::cambiaaccount),
    padre(_padre),
    ElencoServizi(_ElencoServizi),
    ElencoServiziConfigurati(_ElencoServiziConfigurati),
    Rubrica(_Rubrica),
    id(_id),
    found(_found)
{
    m_ui->setupUi(this);

    if (found == 1)
        m_ui->label->setText("E' stato trovato un contatto avente il servizio cancellato come account associato. Si può associare automaticamente questo contatto a un altro servizio.");
    else
        m_ui->label->setText("Sono stati trovati " + QString::number(found) + " contatti aventi il servizio cancellato come account associato. Si possono associare automaticamente questi contatti a un altro servizio.");

    changed = 0;

    spinner = new QMovie(":/resource/loading-spinner.gif", QByteArray("gif"), this);
    spinner->setScaledSize(QSize(16, 16));
    spinner->start();
    m_ui->labelSpin->setMovie(spinner);
    m_ui->labelSpin->hide();

    {
        for(libJackSMS::dataTypes::configuredServicesType::const_iterator i = ElencoServiziConfigurati.begin(); i != ElencoServiziConfigurati.end(); ++i)
            m_ui->comboaccount->addItem(_ElencoServizi[i->getService()].getIcon(),i->getName());

        m_ui->comboaccount->model()->sort(0);
    }
}

cambiaaccount::~cambiaaccount()
{
    delete m_ui;
}

void cambiaaccount::on_buttonSalva_clicked()
{
    stopsearch = false;
    m_ui->buttonSalva->setEnabled(false);
    m_ui->buttonAnnulla->setEnabled(false);
    m_ui->comboaccount->setEnabled(false);
    m_ui->labelSpin->show();
    try
    {
        QString idAccount;
        {
            QString nomeAccount = m_ui->comboaccount->currentText();
            for (libJackSMS::dataTypes::configuredServicesType::const_iterator x = ElencoServiziConfigurati.begin(); x != ElencoServiziConfigurati.end(); ++x)
            {
                if (x.value().getName() == nomeAccount)
                {
                    idAccount = x.value().getId();
                    break;
                }
            }
        }

        saver = new libJackSMS::serverApi::contactManager(padre->current_login_id);
        connect(saver, SIGNAL(contactUpdated(libJackSMS::dataTypes::contact)), this, SLOT(salvataggioOk(libJackSMS::dataTypes::contact)));
        connect(saver, SIGNAL(contactNotUpdated()), this, SLOT(salvataggioKo()));

        for (libJackSMS::dataTypes::phoneBookType::iterator i = Rubrica.begin(); i != Rubrica.end(); ++i)
        {
            if(i->getAccount() == id)
            {
                libJackSMS::dataTypes::contact contatto(i->getName(), i->getPhone(), i->getGroup(), idAccount);
                contatto.setId(i->getId());
                i->setAccount(idAccount);

                if (stopsearch)
                    break;

                saver->updateContact(contatto);
            }
        }

    } catch(...) {
        salvataggioKo();
    }

    //non dovrebbe mai arrivare qui ma per precauzione..
    if (!stopsearch)
        close();
}

void cambiaaccount::on_buttonAnnulla_clicked()
{
    close();
}

void cambiaaccount::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;

    default:
        break;
    }
}

void cambiaaccount::salvataggioOk(libJackSMS::dataTypes::contact c)
{
    Rubrica[c.getId()] = c;
    ++changed;

    if(changed == found)
    {
        padre->ReWriteAddressBookToGui();
        QMessageBox::information(this, "Freesmee", "Contatti modificati.");
        close();
    }
}

void cambiaaccount::salvataggioKo()
{
    stopsearch = true;
    m_ui->buttonSalva->setEnabled(true);
    m_ui->buttonSalva->setEnabled(true);
    m_ui->comboaccount->setEnabled(true);
    m_ui->labelSpin->hide();

    if (found == 1)
        QMessageBox::critical(this, "Freesmee", "Si e' verificato un errore durante l'aggiornamento del contatto. Riprova.");
    else
        QMessageBox::critical(this, "Freesmee", "Si e' verificato un errore durante l'aggiornamento dei contatti. Riprova.");
}
