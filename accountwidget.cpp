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

#include "accountwidget.h"
#include "libjacksms/libJackSMS.h"
#include <QLabel>

accountWidget::accountWidget(QString _id,QString _nome,QPixmap _icoServ):id(_id)
{
       hLayout = new QHBoxLayout;
       //hLayout->setMargin(2);

       nome=new QLabel(_nome);

       //icon->setMaximumSize(1000,16);
       //nome->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
       icon=new QLabel;
       icon->setMaximumSize(16,16);
       //icon->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
       icon->setPixmap(_icoServ);
       hLayout->addWidget(icon);
       hLayout->addWidget(nome);
       setLayout(hLayout);
       //setMaximumSize(1000,16);
       setMinimumSize(200,16);
       setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
       adjustSize();

}
QIcon accountWidget::getIcon()const{
    return QIcon(*icon->pixmap());
}
QString accountWidget::getName()const{
    return nome->text();
}
QString accountWidget::getAccountId()const{

    return id;
}
