#ifndef JACKCAPTCHAPLUGININTERFACER_H
#define JACKCAPTCHAPLUGININTERFACER_H

#include <QtCore>
#include <QtGui>

#include "../JackPluginInterfacer.h"



//questa è un'interfaccia da cui ereditare per implementare un plugin captcha
//e da includere per caricare un plugin di tipo JackCaptchaPluginInterface
class JackCaptchaPluginInterface : public JackPluginInterface
 {
 public:
     virtual ~JackCaptchaPluginInterface() {}

     virtual QString getServiceName() = 0;
     virtual QString decodeCaptcha(QString imagePath) = 0;

 };


//dichiara che questa è un'interfaccia QT
Q_DECLARE_INTERFACE(JackCaptchaPluginInterface, "jackSMS.CaptchaPlugins.VodafoneSMS/1.0")

#endif // JACKCAPTCHAPLUGININTERFACER_H
