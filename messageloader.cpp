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

#include "messageloader.h"
#include "libjacksms/libJackSMS.h"

messageLoader::messageLoader(QString _ud, MainJackSMS* _mainjack) :
        ud(_ud),
        mainjack(_mainjack),
        aborted(false)
{
    qRegisterMetaType<QList <QMyMessage> >("QList<QMyMessage>");
}

void messageLoader::run()
{
    QList<QMyMessage> messaggi;
    libJackSMS::localApi::xmlLoader loader(ud);
    libJackSMS::dataTypes::logImType ArchivioIm;
    libJackSMS::dataTypes::logSmsType ArchivioMessaggi;
    loader.loadImLog(ArchivioIm);
    loader.loadSmsLog(ArchivioMessaggi);

    for (libJackSMS::dataTypes::logImType::const_iterator i = ArchivioIm.begin(); i != ArchivioIm.end(); ++i)
    {
        QMyMessage msg;
        msg.setAccountId(QString::number(0));
        QDateTime data = i.value().getDate().getData();
        msg.setData(data);
        msg.setMessage(i.value().getText());
        msg.setId(i.value().getId());
        msg.setIsReceived(true);
        msg.setPhone(i.value().getPhoneNumber());
        msg.setAccountName("Free+");
        msg.setParsedName(mainjack->phone2name(msg.getPhone()));

        messaggi.push_back(msg);
    }

    for (libJackSMS::dataTypes::logSmsType::const_iterator i = ArchivioMessaggi.begin(); i != ArchivioMessaggi.end(); ++i)
    {
        QMyMessage msg;
        msg.setAccountName(i.value().getAccount());
        QDateTime data = i.value().getDate().getData();
        msg.setData(data);
        msg.setMessage(i.value().getText());
        msg.setId(i.value().getId());
        msg.setIsReceived(false);
        msg.setPhone(i.value().getPhoneNumber());
        msg.setServiceId(i.value().getServiceId());
        msg.setParsedName(mainjack->phone2name(msg.getPhone()));

        messaggi.push_back(msg);

    }

    if (!aborted)
    {
        qSort(messaggi.begin(), messaggi.end(), compareMessages);
        emit messagesLoaded(messaggi);
    }
}

void messageLoader::loadMessages()
{
    start();
}

void messageLoader::abort()
{
    aborted = true;
}

bool messageLoader::compareMessages(QMyMessage &s1, QMyMessage &s2)
{
    return (s1.getData() < s2.getData());
}
