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

#ifndef LISTNAMES_H
#define LISTNAMES_H

#include <QListWidget>
#include "namewidget.h"
#include "mainjacksms.h"
#include "libjacksms/PhoneNumber.h"
#include "libjacksms/Contact.h"

class ListNames : public QListWidget
{
    Q_OBJECT

public:
    explicit ListNames(QWidget *parent = 0);
    void addName(QString name, int unreadCount = 0);
    void addName(QString name, libJackSMS::dataTypes::phoneNumber numero, QString lastMessage, QDateTime lastTime, QString lastId, int unreadCount = 0);
    void insertName(QString name, libJackSMS::dataTypes::phoneNumber numero, QString lastMessage, QDateTime lastTime, QString lastId, int row, int unreadCount = 0);
    void insertName(QString name, int row, int unreadCount = 0);
    void clear();
    void refreshAll(MainJackSMS* main, QListWidget* smslist, bool clean = true);
    void refreshOneBottom(MainJackSMS* main, SmsWidget* sms);
    bool checkNeedRefresh(QString _id);
    void itemAdded(SmsWidget* sms, bool unread);
    NameWidget* findNameWidget(libJackSMS::dataTypes::phoneNumber numero);
    NameWidget* findNameWidget(libJackSMS::dataTypes::contact c);
};

#endif // LISTNAMES_H
