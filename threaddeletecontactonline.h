#ifndef THREADDELETECONTACTONLINE_H
#define THREADDELETECONTACTONLINE_H
#include <QThread>
#include "Types.h"

class threadDeleteContactOnline : public QThread
{
    Q_OBJECT
private:
    libJackSMS::dataTypes::phoneBookType & Rubrica;
    QString sessId;
    QString contactId;
    const libJackSMS::dataTypes::optionsType & opzioni;
public:
    threadDeleteContactOnline(libJackSMS::dataTypes::phoneBookType & _rubrica,QString _sessId,QString _contactId,const libJackSMS::dataTypes::optionsType &_opzioni);
    void run();
signals:
    void deleteOk();
    void deleteKo();
};

#endif // THREADDELETECONTACTONLINE_H
