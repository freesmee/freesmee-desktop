#ifndef JACKSCRIBAPLUGININTERFACER_H
#define JACKSCRIBAPLUGININTERFACER_H

#include <QtCore>
#include <QtGui>

#include "../JackPluginInterfacer.h"
#include "../JackPluginHostInterfacer.h"



//questa è un'interfaccia da cui ereditare per implementare un plugin puppetter (burattinaio)
//questo tipo di plugins ha un riferimento all'istanza di JackPluginHostInterface e puo' pertanto ascoltare i messaggi in arrivo ed eseguire varie operazioni.
class JackPuppeteerPluginInterface : public JackPluginInterface
 {
 private:
    bool initialized = false;
 public:
     virtual ~JackPuppeteerPluginInterface() {}
     virtual initialize(JackPluginHostInterface*);
 };


//dichiara che questa è un'interfaccia QT
Q_DECLARE_INTERFACE(JackPuppeteerPluginInterface, "jackSMS.PuppeteerPlugins.VodafoneSMS/1.0")

#endif // JACKSCRIBAPLUGININTERFACER_H
