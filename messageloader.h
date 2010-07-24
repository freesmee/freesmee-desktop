#ifndef MESSAGELOADER_H
#define MESSAGELOADER_H
#include "qmymessage.h"

#include <QThread>

class messageLoader : public QThread
{
    Q_OBJECT
private:
    void run();
    QString ud;
    bool aborted;
public:
    messageLoader(QString _userDir);
    void loadMessages();
    void abort();
signals:
    void messagesLoaded(QList<QMyMessage>);
};

#endif // MESSAGELOADER_H
