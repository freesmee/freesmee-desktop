#include "JackPluginHostInterfacer.h"



void JackPluginHostInterface::emitJIMreceived(libJackSMS::dataTypes::logImMessage im){
    emit JIMreceived(im);
}
