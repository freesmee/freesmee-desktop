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

#ifndef ACCOUNTWIDGET_H
#define ACCOUNTWIDGET_H

#include <QWidget>
#include <QLabel>

#include <QHBoxLayout>
#include "libjacksms/libJackSMS.h"

class accountWidget : public QWidget
{
private:

    QLabel *nome,*icon;
    QHBoxLayout *hLayout;
    QString id;


public:
    accountWidget(QString _id,QString _nome,QPixmap _icoServ);

    QString getContactId()const;
    QString getAccountId()const;
    QIcon getIcon() const ;
    QString getName()const;
};

#endif // ACCOUNTWIDGET_H
