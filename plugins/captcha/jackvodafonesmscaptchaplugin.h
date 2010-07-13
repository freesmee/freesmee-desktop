#ifndef JACKVODAFONESMSCAPTCHAPLUGIN_H
#define JACKVODAFONESMSCAPTCHAPLUGIN_H

#ifdef __WIN32__
#define CONVERT_EXE "convert.exe"
#define  TESSERACT_EXE "tesseract.exe"

#else
#define CONVERT_EXE "convert"
#define  TESSERACT_EXE "tesseract"
#endif

#include <QtCore>
#include <QtGui>

#include "JackCaptchaPluginInterfacer.h"

class JackVodafoneSMSCaptchaPlugin : public JackCaptchaPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(JackCaptchaPluginInterface)


public:
    JackVodafoneSMSCaptchaPlugin();

    QString getServiceName();
    QString decodeCaptcha(QString);
    PluginStatus getPluginStatus();
    PluginInfo getPluginInfo();
public slots:
    void configure();
};

#endif // JACKVODAFONESMSCAPTCHAPLUGIN_H
