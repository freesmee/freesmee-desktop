#include "threadcheckupdates.h"

threadCheckUpdates::threadCheckUpdates(libJackSMS::dataTypes::servicesType &_servizi,const libJackSMS::dataTypes::optionsType &_opzioni,QString _loginId)
    :servizi(_servizi),opzioni(_opzioni),loginId(_loginId){
}

void threadCheckUpdates::run(){
    try{
        libJackSMS::serverApi::updateServicesManager man(loginId,opzioni);
        if (man.downloadUpdates(servizi)){
            result=man.getMessage();
            libJackSMS::localApi::serviceManager saver;

            saver.mergeServices(man.getXml());
            emit updatesAvailable();
        }
    }catch(...){
        emit fatalError();
    }


}
QString threadCheckUpdates::getResult()const{
    return result;
}
