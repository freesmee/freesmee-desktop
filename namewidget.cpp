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

#include "namewidget.h"

NameWidget::NameWidget(QString _name, libJackSMS::dataTypes::phoneNumber _numero, QString _lastMessage, QDateTime _lastTime, QString _lastId, int _unreadCount)
{
    setStyleSheet("NameWidget{"
                  "background-image: url(:/resource/bg_namewidget.png);"
                  "background-repeat:repeat-x;"
                  "}");

    unreadCount = _unreadCount;
    name = _name;
    numero = _numero;

    if(_name.length() > 20)
        _name = _name.left(18) + "...";

    labelName = new QLabel(_name);
    labelName->setFont(QFont(labelName->font().family(), -1, QFont::Bold, false));
    labelName->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    labelName->setFixedSize(150, 15);

    lastMessage = _lastMessage;
    if(_lastMessage.length() > 22)
        _lastMessage = _lastMessage.left(20) + "...";

    labelLastMessage = new QLabel(_lastMessage);
    labelLastMessage->setFont(QFont(labelLastMessage->font().family(), -1, QFont::Normal, false));
    labelLastMessage->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    labelLastMessage->setFixedSize(150, 15);

    lastTime = _lastTime;
    lastId = _lastId;

    labelIconStatus = new QLabel();

    if(unreadCount == 0)
        labelIconStatus->setPixmap(QPixmap(":/resource/arrow.png"));
    else
        labelIconStatus->setPixmap(QPixmap(":/resource/arrow-unread.png"));

    labelIconStatus->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    labelIconStatus->setFixedSize(16, 16);

    hLayout = new QHBoxLayout;
    hLayout->setMargin(5);
    vLayout = new QVBoxLayout;
    vLayout->setMargin(5);

    labelName->adjustSize();
    labelLastMessage->adjustSize();
    labelIconStatus->adjustSize();

    vLayout->addWidget(labelName);
    vLayout->addWidget(labelLastMessage);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(labelIconStatus);

    setLayout(hLayout);
    setFixedWidth(185);
    adjustSize();
}

QString NameWidget::getName() const
{
    return name;
}

void NameWidget::setName(QString newname)
{
    name = newname;

    if(newname.length() > 16)
        newname = newname.left(13) + "...";

    labelName->setText(newname);
}

libJackSMS::dataTypes::phoneNumber NameWidget::getPhoneNum() const
{
    return numero;
}

QString NameWidget::getId() const
{
    return lastId;
}

QDateTime NameWidget::getTime() const
{
    return lastTime;
}

int NameWidget::getUnreadCount() const
{
    return unreadCount;
}

void NameWidget::increaseUnreadCount()
{
    if(unreadCount == 0)
        setIconUnread();

    unreadCount++;
}

void NameWidget::decreaseUnreadCount()
{
    if(unreadCount <= 0)
        return;

    unreadCount--;
    if(unreadCount == 0)
        setIconRead();
}

void NameWidget::setUnreadCount(int _unreadcount){
    if(_unreadcount < 0)
        unreadCount = 0;
    else
        unreadCount = _unreadcount;

    if(unreadCount == 0)
        setIconRead();
    else
        setIconUnread();
}

void NameWidget::setIconUnread(){
    labelIconStatus->setPixmap(QPixmap(":/resource/arrow-unread.png"));
}

void NameWidget::setIconRead(){
    labelIconStatus->setPixmap(QPixmap(":/resource/arrow.png"));
}
