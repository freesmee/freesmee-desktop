#ifndef THREADSAVECONTACTONLINE_H
#define THREADSAVECONTACTONLINE_H
//#include "Types.h"
#include <QThread>
#include "libjacksms/libJackSMS.h"
class threadSaveContactOnline : public QThread
{
      Q_OBJECT

private:
    libJackSMS::dataTypes::phoneBookType & Rubrica;
    QString sessId;
    libJackSMS::dataTypes::contact contatto;
    const libJackSMS::dataTypes::optionsType & opzioni;
public:

    threadSaveContactOnline(libJackSMS::dataTypes::phoneBookType & _rubrica,QString _sessId,libJackSMS::dataTypes::contact & _contatto,const libJackSMS::dataTypes::optionsType &_opzioni);
    void run();
signals:
    void saveOk();
    void saveKo();
};

#endif // THREADSAVECONTACTONLINE_H
