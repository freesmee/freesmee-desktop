#ifndef THREADSAVESMSONLINE_H
#define THREADSAVESMSONLINE_H

#include <QThread>
#include "libjacksms/libJackSMS.h"
class threadSaveSmsOnline : public QThread
{
    Q_OBJECT
private:
    QString loginId;
    libJackSMS::dataTypes::logSmsMessage msg;
    const libJackSMS::dataTypes::optionsType & opzioni;
    QString userDir;
    QString accountId;
public:
    threadSaveSmsOnline(QString loginId,libJackSMS::dataTypes::logSmsMessage _msg,const libJackSMS::dataTypes::optionsType &_opzioni,QString _userDir,QString _accountId);
    void run();
signals:
    void smsSaved();
    void smsNotSaved();
    void fatalError();
};

#endif // THREADSAVESMSONLINE_H
