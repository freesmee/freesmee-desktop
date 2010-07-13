#include "JackScribaCompressore.h"

JackScribaCompressore::JackScribaCompressore(){

}

PluginStatus JackScribaCompressore::getPluginStatus(){
    return Ready;
}

PluginInfo JackScribaCompressore::getPluginInfo(){
    PluginInfo info;
    info.name = "Scriba: Compressore";
    info.author = "Master_T";
    info.pluginType = Scriba;
    info.version = "0.1";
    info.description = "Un plugin che effettua sostituzioni nel testo per abbreviare il messaggio";

    return info;
}

void JackScribaCompressore::processText(QString& t){
    //just an example to test the plugin
    t.replace("ch", "k");
    t.replace("sei", "6");
    t.replace("comunque", "cmq");
    //ecc...
}

void JackScribaCompressore::configure(){
    QMessageBox::information(0, "Info", "Nessuna opzione da configurare per ora!");
}

Q_EXPORT_PLUGIN2(JackScribaCompressore, JackScribaCompressore)
