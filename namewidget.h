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

#ifndef NAMEWIDGET_H
#define NAMEWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include "libjacksms/PhoneNumber.h"

class NameWidget : public QFrame
{
    Q_OBJECT

public:
    NameWidget(QString _name, libJackSMS::dataTypes::phoneNumber _numero, QString _lastMessage, QDateTime _lastTime, QString _lastId, int unreadCount = 0);
    QString getName() const;
    void setName(QString newname);
    libJackSMS::dataTypes::phoneNumber getPhoneNum() const;
    QString getId() const;
    QDateTime getTime() const;
    int getUnreadCount() const;
    void increaseUnreadCount();
    void decreaseUnreadCount();
    void setUnreadCount(int _unreadcount);

private:
    QLabel *labelIconStatus, *labelName, *labelLastMessage, *labelLastTime;
    QString name, lastMessage, lastId;
    libJackSMS::dataTypes::phoneNumber numero;
    QDateTime lastTime;
    QVBoxLayout *vLayout;
    QHBoxLayout *hLayout;
    int unreadCount;
    void setIconUnread();
    void setIconRead();
};

#endif // NAMEWIDGET_H
