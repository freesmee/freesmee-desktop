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

#ifndef SMSLIST_H
#define SMSLIST_H

#include <QListWidget>
#include <QKeyEvent>
#include <QClipboard>
#include <QApplication>
#include "smswidget.h"
#include "libjacksms/PhoneNumber.h"
#include "libjacksms/Contact.h"
#include <QString>

class SmsList : public QListWidget
{
    Q_OBJECT
public:
    explicit SmsList(QWidget *parent = 0);
    void hideCaricaAltri(bool hide);
    void addItem(QListWidgetItem *item);
    void clear();
    void changeNameForGivenNumber(libJackSMS::dataTypes::phoneNumber numero, QString newname);
    void changeNameForGivenContact(libJackSMS::dataTypes::contact c, QString newname);
    void setItemHidden(const QListWidgetItem *item, bool hide);

private:
    void keyPressEvent(QKeyEvent* e);

signals:
    void smsListCanc();

public slots:

};

#endif // SMSLIST_H
