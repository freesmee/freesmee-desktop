#ifndef JACKPLUGINHOSTINTERFACER_H
#define JACKPLUGINHOSTINTERFACER_H

#include <QtCore>
#include <QtGui>
#include "libjacksms/libJackSMS.h"

//questa è un'interfaccia da cui ereditare per implementare un plugin host
//JackSMS implementa questa interfaccia
class JackPluginHostInterface : public QObject

{
    Q_OBJECT

public:
     JackPluginHostInterface() {}
     void emitJIMreceived(libJackSMS::dataTypes::logImMessage);

 signals:
     void JIMreceived(libJackSMS::dataTypes::logImMessage);

 };

class JackPluginMethodProvider{
public:
    //bool sendMessage();
    //ecc...
};


#endif // JACKPLUGINHOSTINTERFACER_H
