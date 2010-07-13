#include "threadpingserver.h"

threadPingServer::threadPingServer(QString _sessId,const libJackSMS::dataTypes::optionsType &_opzioni):
        sessId(_sessId),opzioni(_opzioni)
{
}

void threadPingServer::run(){

    try{

        libJackSMS::serverApi::pingator ping(sessId,opzioni);
        ping.ping();
        emit this->pinged();
   }catch(libJackSMS::exceptionXmlError e){
       error=e.what();
       result=false;
   }catch(libJackSMS::exceptionSomethingWrong e){
       error=e.what();
       result=false;
   }catch(...){
       error=QString("Errore sconosciuto");
       result=false;
   }
}
