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

#include "contactwidget.h"
#include <QLabel>

ContactWidget::ContactWidget(QString _id, QString _nome, QString _numero, QPixmap _icoServ, QString _gruppo, QString _accountName) :
        id(_id),
        nomeContatto(_nome)
{
       hLayout = new QHBoxLayout;
       //vLayout = new QVBoxLayout;
       nome = new QLabel(_nome);
       //nome->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
       numero = new QLabel(_numero);
       icon = new QLabel;
       icon->setMaximumSize(16, 200);
       icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
       icon->setPixmap(QPixmap(":/resource/ico_contact.png"));
       nomeAccount = new QLabel(_accountName);
       //numero->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
       iconServizio = new QLabel;
       iconServizio->setPixmap(_icoServ);
       iconServizio->setMaximumSize(16, 200);
       iconServizio->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

       // Da rimettere quando implementiamo i gruppi
       //gruppo=new QLabel(_gruppo);
       //gruppo->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

       hLayout->addWidget(icon);
       hLayout->addWidget(nome);
       hLayout->addWidget(numero);
       hLayout->addWidget(iconServizio);
       hLayout->addWidget(nomeAccount);

       // Da rimettere quando implementiamo i gruppi
       //hLayout->addWidget(gruppo);

       //vLayout->addLayout(hLayout);
       setLayout(hLayout);
       setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
       adjustSize();
}

QString ContactWidget:: getName() const {
    return nomeContatto;
}

QString ContactWidget:: getContactId() const {
    return id;
}

bool ContactWidget::searchMatch(QString _txt) const {
    return (!(-1 == nome->text().indexOf(_txt, 0, Qt::CaseInsensitive))) || (!(-1 == numero->text().indexOf(_txt, 0, Qt::CaseInsensitive))) || (!(-1 == nomeAccount->text().indexOf(_txt, 0, Qt::CaseInsensitive)));
}
