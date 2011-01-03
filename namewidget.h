#ifndef NAMEWIDGET_H
#define NAMEWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>

class NameWidget : public QFrame
{
    Q_OBJECT

public:
    NameWidget(QString _name, QString _lastMessage, QDateTime _lastTime, QString _lastId, int unreadCount = 0);
    QString getName() const;
    QString getId() const;
    QDateTime getTime() const;
    int getUnreadCount() const;
    void increaseUnreadCount();
    void decreaseUnreadCount();
    void setUnreadCount(int _unreadcount);

private:
    QLabel *labelIconStatus, *labelName, *labelLastMessage, *labelLastTime;
    QString name, lastMessage, lastId;
    QDateTime lastTime;
    QVBoxLayout *vLayout;
    QHBoxLayout *hLayout;
    int unreadCount;
    void setIconUnread();
    void setIconRead();
};

#endif // NAMEWIDGET_H
