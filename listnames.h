#ifndef LISTNAMES_H
#define LISTNAMES_H

#include <QListWidget>
#include "namewidget.h"
#include "mainjacksms.h"

class ListNames : public QListWidget
{
    Q_OBJECT

public:
    explicit ListNames(QWidget *parent = 0);
    void addName(QString name, int unreadCount = 0);
    void addName(QString name, QString lastMessage, QDateTime lastTime, QString lastId, int unreadCount = 0);
    void insertName(QString name, QString lastMessage, QDateTime lastTime, QString lastId, int row, int unreadCount = 0);
    void insertName(QString name, int row, int unreadCount = 0);
    void svuota();
    void refreshAll(MainJackSMS* main, QListWidget* smslist, bool clean = true);
    void nascondiCaricaAltriMessaggi();
    bool checkNeedRefresh(QString _id, QListWidget* smslist);
    void itemAdded(SmsWidget* sms, bool unread);

private:
    bool varNascondiCaricaAltriMessaggi;
};

#endif // LISTNAMES_H
