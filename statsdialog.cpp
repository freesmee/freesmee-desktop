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

#include "statsdialog.h"
#include "ui_statsdialog.h"
#include <QLabel>
#include <QMap>
#include <QPixmap>
#include <QTextEdit>
#include "libjacksms/libJackSMS.h"
#include <QHBoxLayout>
#include "libjacksms/ConfiguredAccount.h"

StatsDialog::StatsDialog(const libJackSMS::dataTypes::optionsType &_opzioni, const libJackSMS::dataTypes::configuredServicesType &_servizi, const libJackSMS::dataTypes::servicesType &_ElencoServizi, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::StatsDialog),
    opzioni(_opzioni),
    servizi(_servizi),
    ElencoServizi(_ElencoServizi)
{
    m_ui->setupUi(this);
    int tot= 0;

    QMap<QString, libJackSMS::dataTypes::configuredAccount> tempMap;
    for(libJackSMS::dataTypes::configuredServicesType::const_iterator i = servizi.begin(); i != servizi.end(); ++i) {
        if (i.value().getId() != "1")
            tempMap.insert(i.value().getName(), i.value());
    }

    // Aggiungo Jacksms Messenger separatamente in modo che sia in cima alla lista
    libJackSMS::dataTypes::configuredServicesType::const_iterator i = servizi.find("1");
    QHBoxLayout *hLayout = new QHBoxLayout;
    {
        QLabel *l = new QLabel;
        l->setPixmap(ElencoServizi[i.value().getService()].getIcon().pixmap(16,16));
        l->setFixedSize(16,16);
        hLayout->addWidget(l);
    }
    {
        QTextEdit *l = new QTextEdit;

        QString result = "<b>" + i.value().getName() + "</b><br>Inviati oggi da tutti i client: " + QString::number(i.value().getSentAll()) + "<br>Totale inviati da questa postazione: " + i.value().getStat("sent");
        tot += i.value().getStat("sent").toInt();
        l->setText(result);
        l->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);
        l->setMaximumHeight(50);
        l->setReadOnly(true);
        hLayout->addWidget(l);
    }

    hLayout->setSizeConstraint(QLayout::SetMinimumSize);
    m_ui->layoutStatistiche->addLayout(hLayout);

    for (libJackSMS::dataTypes::configuredServicesType::const_iterator i = tempMap.begin(); i != tempMap.end(); ++i) {
        QHBoxLayout *hLayout = new QHBoxLayout;
        {
            QLabel *l = new QLabel;
            l->setPixmap(ElencoServizi[i.value().getService()].getIcon().pixmap(16, 16));
            l->setFixedSize(16, 16);
            hLayout->addWidget(l);
        }
        {
            QTextEdit *l = new QTextEdit;

            QString result = "<b>" + i.value().getName() + "</b><br>Inviati oggi da tutti i client: " + QString::number(i.value().getSentAll()) + "<br>Totale inviati da questa postazione: " + i.value().getStat("sent");
            tot += i.value().getStat("sent").toInt();
            l->setText(result);
            l->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);
            l->setMaximumHeight(50);
            l->setReadOnly(true);
            hLayout->addWidget(l);
        }
        m_ui->layoutStatistiche->addLayout(hLayout);
    }

    m_ui->labelTotal->setText("Messaggi inviati da questa postazione: <b>" + QString::number(tot) + "</b>");
}

StatsDialog::~StatsDialog()
{
    delete m_ui;
}

void StatsDialog::changeEvent(QEvent *e)
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
