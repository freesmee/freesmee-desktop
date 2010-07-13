#ifndef THREADUPDATECONTACT_H
#define THREADUPDATECONTACT_H

#include <QThread>
#include "libjacksms/libJackSMS.h"
class threadUpdateContact : public QThread
{
     Q_OBJECT
private:
    libJackSMS::dataTypes::phoneBookType & Rubrica;
    QString sessId;
    libJackSMS::dataTypes::contact contatto;
    const libJackSMS::dataTypes::optionsType & opzioni;
public:
    threadUpdateContact(libJackSMS::dataTypes::phoneBookType & _rubrica,QString _sessId,libJackSMS::dataTypes::contact & _contatto,const libJackSMS::dataTypes::optionsType &_opzioni);
    void run();
signals:
    void updateOk();
    void updateKo();
};

#endif // THREADUPDATECONTACT_H
