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

#ifndef CONTACTWIDGET_H
#define CONTACTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ContactWidget : public QWidget
{
private:

    QLabel *nome,*numero,*iconServizio,*icon,*gruppo,*nomeAccount;
    QHBoxLayout *hLayout;
    QString id;
    QString nomeContatto;

public:
    ContactWidget(QString _id,QString _nome,QString _numero,QPixmap _icoServ,QString _gruppo,QString _accountName);
    QString getName()const;
    QString getContactId()const;
    bool searchMatch(QString _txt)const;
};

#endif // CONTACTWIDGET_H
