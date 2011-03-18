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
    bool checkNeedRefresh(QString _id, QListWidget* smslist);
    void itemAdded(SmsWidget* sms, bool unread);
    NameWidget* findNameWidget(libJackSMS::dataTypes::phoneNumber numero);
    NameWidget* findNameWidget(libJackSMS::dataTypes::contact c);
};

#endif // LISTNAMES_H
