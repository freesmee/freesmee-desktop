/*
    Copyright (C) <2011>

    <enrico bacis> <enrico.bacis@gmail.com>
    <ivan vaccari> <grisson@jacksms.it>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "contactwidgetfastbook.h"
#include "libjacksms/libJackSMS.h"
#include <QLabel>
#include <QCheckBox>

contactWidgetFastBook::contactWidgetFastBook(const libJackSMS::dataTypes::contact &_c, QPixmap _icoServ, bool addCheckBox) :
        id(_c.getId()),
        nomeContatto(_c.getName()),
        accountId(_c.getAccount()),
        numero(_c.getPhone().toString()),
        c(_c)
{
       hLayout = new QHBoxLayout;
       hLayout->setMargin(2);
       labelIconInfo = new QLabel;

       nome = new QLabel(_c.getName());

       QString tooltip = QString(numero);
       QString carrier = _c.getCarrierString();

       if (!carrier.isEmpty())
           tooltip = tooltip + "  (" + carrier + ")";

       nome->setToolTip(tooltip);

       icon = new QLabel;
       icon->setMaximumSize(16, 16);
       icon->setPixmap(_icoServ);

       if (addCheckBox) {
           chbox = new QCheckBox;
           chbox->setChecked(false);
           chbox->setMaximumSize(16, 16);
           hLayout->addWidget(chbox);
       }

       hLayout->addWidget(icon);
       hLayout->addWidget(nome);
       setLayout(hLayout);
       setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
       adjustSize();
}

libJackSMS::dataTypes::contact contactWidgetFastBook::getContact() const {
    return c;
}

QPixmap contactWidgetFastBook::getIcon() const {
    return  QPixmap(*icon->pixmap());
}

void contactWidgetFastBook::setIcon(QPixmap _pix) {
    icon->setPixmap(_pix);
}

void contactWidgetFastBook::showIcon(bool value) {
    if(value)
        icon->show();
    else
        icon->hide();
    return;
}

QString contactWidgetFastBook:: getName() const {
    return nomeContatto;
}

QString contactWidgetFastBook:: getContactId() const {
    return id;
}

contactWidgetFastBook *contactWidgetFastBook::clone() const {
    QPixmap p(*icon->pixmap());
    return new contactWidgetFastBook(c,p);
}

bool contactWidgetFastBook::isChecked() const {
    return chbox->isChecked();
}

void contactWidgetFastBook::setChecked(bool value) {
    chbox->setChecked(value);
}

bool contactWidgetFastBook:: searchMatch(QString _txt) const {
    return (!(-1 == nomeContatto.indexOf(_txt, 0, Qt::CaseInsensitive))) || (!(-1 == numero.indexOf(_txt, 0, Qt::CaseInsensitive))) ;
}
