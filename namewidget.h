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
