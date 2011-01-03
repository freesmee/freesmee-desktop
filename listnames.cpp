#include "listnames.h"

ListNames::ListNames(QWidget *parent) :
    QListWidget(parent)
{
    svuota();
}

void ListNames::addName(QString name, int unreadCount)
{
    addName(name, "", QDateTime(), "", unreadCount);
}

void ListNames::addName(QString name, QString lastMessage, QDateTime lastTime, QString lastId, int unreadCount)
{
    NameWidget* namewid = new NameWidget(name, lastMessage, lastTime, lastId, unreadCount);
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(QSize(100,18+18+5+5+5));
    addItem(item);
    setItemWidget(item, namewid);
}

void ListNames::insertName(QString name, QString lastMessage, QDateTime lastTime, QString lastId, int row, int unreadCount){
    NameWidget* namewid = new NameWidget(name, lastMessage, lastTime, lastId, unreadCount);
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(QSize(100,18+18+5+5+5));
    insertItem(row, item);
    setItemWidget(item, namewid);
}

void ListNames::insertName(QString name, int row, int unreadCount)
{
    insertName(name, "", QDateTime(), "", row, unreadCount);
}

void ListNames::svuota()
{
    clear();
    addName("Tutti i contatti");
}

void ListNames::refreshAll(MainJackSMS* main, QListWidget* smslist)
{
    svuota();

    QString nameToInsert;
    bool alreadyFound;
    SmsWidget* sms;
    NameWidget* namewid;

    for(int j = 0; j < smslist->count(); j++){
        sms = static_cast<SmsWidget*>(smslist->itemWidget(smslist->item(j)));
        nameToInsert = main->phone2name(sms->getPhoneNum());

        alreadyFound = false;
        for(int i = 1; i < count(); ++i){
            namewid = static_cast<NameWidget*>(itemWidget(item(i)));
            if(namewid->getName() == nameToInsert){
                alreadyFound = true;
                if(!sms->isReaded())
                    namewid->increaseUnreadCount();

                break;
            }
        }
        if(!alreadyFound)
            addName(nameToInsert, sms->getText(), sms->getDateTime(), sms->getId(), (sms->isReaded() ? 0 : 1));

    }
}

void ListNames::nascondiCaricaAltriMessaggi(){
    varNascondiCaricaAltriMessaggi = false;
}

bool ListNames::checkNeedRefresh(QString _id, QListWidget* smslist){

    // scorro la listanomi (il primo no perchè è "Tutti i contatti")
    for(int i = 1; i < count(); i++){
        NameWidget* namewid = static_cast<NameWidget*>(itemWidget(item(i)));

        // vedo se c'è un namewidget che usa il messaggio eliminato
        if(namewid->getId() == _id){

            /*
            // se trovo un namewidget che usava il messaggio cancellato cerco un sostituto per la lista nomi
            // scorro la lista sms
            for(int j = 0; j < smslist->count(); j++){
                SmsWidget* smswid = static_cast<SmsWidget*>(smslist->itemWidget(smslist->item(j)));

                // se il nome legato all'sms è uguale a quello del namewidget cancellato potrei aver trovato un sostituto
                if(namewid->getName() == smswid->getName()){

                    // controllo che non sia il messaggio che devo cancellare
                    if(namewid->getId() != smswid->getId()){
                        return true;
                    }
                }
            }*/

            return true;
        }
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
                unreadToSet++;

            it = item(i);
            wid = itemWidget(it);
            takeItem(i);
            delete it;
            delete wid;
            break;
        }
    }
    //aggiungo un nuovo nome
    insertName(sms->getName(), sms->getText(), sms->getDateTime(), sms->getId(), 1, unreadToSet);

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
