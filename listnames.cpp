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

#include "listnames.h"

ListNames::ListNames(QWidget *parent) :
    QListWidget(parent)
{
    addName("Tutti i contatti");
}

void ListNames::addName(QString name, int unreadCount)
{
    //workaround
    libJackSMS::dataTypes::phoneNumber ph;
    ph.parse("+00.00.000000");

    addName(name, ph, "", QDateTime(), "", unreadCount);
}

void ListNames::addName(QString name, libJackSMS::dataTypes::phoneNumber numero, QString lastMessage, QDateTime lastTime, QString lastId, int unreadCount)
{
    NameWidget* namewid = new NameWidget(name, numero, lastMessage, lastTime, lastId, unreadCount);
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(QSize(100, 18+18+5+5+5));
    addItem(item);
    setItemWidget(item, namewid);
}

void ListNames::insertName(QString name, libJackSMS::dataTypes::phoneNumber numero, QString lastMessage, QDateTime lastTime, QString lastId, int row, int unreadCount){
    NameWidget* namewid = new NameWidget(name, numero, lastMessage, lastTime, lastId, unreadCount);
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(QSize(100, 18+18+5+5+5));
    insertItem(row, item);
    setItemWidget(item, namewid);
}

void ListNames::insertName(QString name, int row, int unreadCount)
{
    //workaround
    libJackSMS::dataTypes::phoneNumber ph;
    ph.parse("+00.00.000000");

    insertName(name, ph, "", QDateTime(), "", row, unreadCount);
}

void ListNames::clear()
{
    clearSelection();
    scrollToTop();
    QListWidget::clear();
    addName("Tutti i contatti");
}

void ListNames::refreshAll(MainJackSMS* main, QListWidget* smslist, bool clean)
{
    if (clean)
        clear();

    QString nameToInsert;
    bool alreadyFound;
    SmsWidget* sms;
    NameWidget* namewid;

    for (int j = 0; j < smslist->count(); ++j) {
        sms = static_cast<SmsWidget*>(smslist->itemWidget(smslist->item(j)));
        if (!sms->isCaricaAltri()) {
            nameToInsert = main->phone2name(sms->getPhoneNum());

            alreadyFound = false;
            for (int i = 1; i < count(); ++i) {
                namewid = static_cast<NameWidget*>(itemWidget(item(i)));
                if (namewid->getName() == nameToInsert) {
                    alreadyFound = true;
                    if (!sms->isReaded())
                        namewid->increaseUnreadCount();

                    break;
                }
            }
            if(!alreadyFound)
                addName(nameToInsert, sms->getPhoneNum(), sms->getText(), sms->getDateTime(), sms->getId(), (sms->isReaded() ? 0 : 1));
        }
    }
}

void ListNames::refreshOneBottom(MainJackSMS* main, SmsWidget *sms)
{
    if (sms->isCaricaAltri())
        return;

    QString nameToInsert = main->phone2name(sms->getPhoneNum());
    bool alreadyFound = false;
    NameWidget* namewid;

    for (int i = 1; i < count(); ++i) {
        namewid = static_cast<NameWidget*>(itemWidget(item(i)));
        if (namewid->getName() == nameToInsert) {
            alreadyFound = true;
            if (!sms->isReaded())
                namewid->increaseUnreadCount();
            break;
        }
    }

    if(!alreadyFound)
        addName(nameToInsert, sms->getPhoneNum(), sms->getText(), sms->getDateTime(), sms->getId(), (sms->isReaded() ? 0 : 1));
}

bool ListNames::checkNeedRefresh(QString _id)
{
    // scorro la listanomi (il primo no perchè è "Tutti i contatti")
    for (int i = 1; i < count(); ++i)
    {
        NameWidget* namewid = static_cast<NameWidget*>(itemWidget(item(i)));

        // vedo se c'è un namewidget che usa il messaggio eliminato
        if(namewid->getId() == _id)
            return true;

    }
    return false;
}

void ListNames::itemAdded(SmsWidget* sms, bool unread){
    QListWidgetItem* it;
    QWidget* wid;
    int unreadToSet = 0;

    //cancello il namewidget con lo stesso nome
    for(int i = 1; i < count(); ++i){
        NameWidget* namewid = static_cast<NameWidget*>(itemWidget(item(i)));
        if(sms->getName() == namewid->getName()){

            unreadToSet = namewid->getUnreadCount();
            if(unread)
                ++unreadToSet;

            it = item(i);
            wid = itemWidget(it);
            takeItem(i);
            delete it;
            delete wid;
            break;
        }
    }
    //aggiungo un nuovo nome
    insertName(sms->getName(), sms->getPhoneNum(), sms->getText(), sms->getDateTime(), sms->getId(), 1, unreadToSet);

    //se unread allora setto l'unread anche su "tutti i contatti"
    if(unread){
        it = item(0);
        wid = itemWidget(it);
        int unreadAllToSet = static_cast<NameWidget*>(wid)->getUnreadCount() + 1;
        takeItem(0);
        delete it;
        delete wid;

        insertName("Tutti i contatti", 0, unreadAllToSet);
    }

}

NameWidget* ListNames::findNameWidget(libJackSMS::dataTypes::phoneNumber numero) {
    NameWidget* result = NULL;

    //parto dal primo perchè lo zeresimo è Tutti i Contatti
    for (int i = 1; i < count(); ++i) {
        NameWidget* namewid = static_cast<NameWidget*>(itemWidget(item(i)));
        if (namewid->getPhoneNum() == numero) {
            result = namewid;
            break;
        }
    }

    return result;
}

NameWidget* ListNames::findNameWidget(libJackSMS::dataTypes::contact c) {
    NameWidget* result = NULL;

    //parto dal primo perchè lo zeresimo è Tutti i Contatti
    for (int i = 1; i < count(); ++i) {
        NameWidget* namewid = static_cast<NameWidget*>(itemWidget(item(i)));
        if (namewid->getPhoneNum() == c.getPhone())
        {
            result = namewid;
            break;
        }
    }

    return result;
}
