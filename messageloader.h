#ifndef MESSAGELOADER_H
#define MESSAGELOADER_H

#include <QThread>
#include "mainjacksms.h"

class messageLoader : public QThread
{
    Q_OBJECT
private:
    void run();
    QString ud;
    MainJackSMS* mainjack;
    bool aborted;
    static bool compareMessages(QMyMessage &s1, QMyMessage &s2);
public:
    messageLoader(QString _userDir, MainJackSMS* _mainjack);
    void loadMessages();
    void abort();
signals:
    void messagesLoaded(QList<QMyMessage>);
};

#endif // MESSAGELOADER_H
