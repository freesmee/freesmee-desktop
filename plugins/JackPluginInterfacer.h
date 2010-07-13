#ifndef JACKPLUGININTERFACER_H
#define JACKPLUGININTERFACER_H

#include <QtCore>
#include <QtGui>



enum PluginStatus{Ready, Broken};
enum PluginType{CaptchaDecoder, Scriba};

struct PluginInfo{
    PluginType pluginType;
    QString name;
    QString author;
    QString description;
    QString version;

    QString pluginTypeAsString(){
        switch (pluginType){
        case CaptchaDecoder:
            return "CaptchaDecoder";
        default:
            return "Unknown/Test";
        }
    }
};



//questa è un'interfaccia da cui ereditare per implementare un plugin
//e da includere per caricare un plugin di JackSMS
class JackPluginInterface : public QObject

{
    Q_OBJECT
 public:
     virtual ~JackPluginInterface() {}

     virtual PluginStatus getPluginStatus() = 0;
     virtual PluginInfo getPluginInfo() = 0;
 public slots:
     virtual void configure() = 0;
 };


//dichiara che questa è un'interfaccia QT
Q_DECLARE_INTERFACE(JackPluginInterface, "jackSMS.Plugins/1.0")

#endif // JACKPLUGININTERFACER_H
