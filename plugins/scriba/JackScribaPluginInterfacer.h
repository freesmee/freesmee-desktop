#ifndef JACKSCRIBAPLUGININTERFACER_H
#define JACKSCRIBAPLUGININTERFACER_H

#include <QtCore>
#include <QtGui>

#include "../JackPluginInterfacer.h"



//questa è un'interfaccia da cui ereditare per implementare un plugin scriba
//questo tipo di plugins applica modifiche al testo dell'sms prima dell'invio
class JackScribaPluginInterface : public JackPluginInterface
 {
 public:
     virtual ~JackScribaPluginInterface() {}

     virtual void processText(QString&) = 0;

 };


//dichiara che questa è un'interfaccia QT
Q_DECLARE_INTERFACE(JackScribaPluginInterface, "jackSMS.CaptchaPlugins.VodafoneSMS/1.0")

#endif // JACKSCRIBAPLUGININTERFACER_H
