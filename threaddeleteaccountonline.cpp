#include "threaddeleteaccountonline.h"

threadDeleteAccountOnline::threadDeleteAccountOnline(libJackSMS::dataTypes::configuredServicesType & _Esc,QString _sessId,QString _accountId,const libJackSMS::dataTypes::optionsType &_opzioni):
        Esc(_Esc),sessId(_sessId),accountId(_accountId),opzioni(_opzioni)
{
}

void threadDeleteAccountOnline::run(){
    try{
        libJackSMS::serverApi::accountManager manager(sessId,opzioni);
        if (manager.deleteAccount(accountId)){
            QString aid=accountId;
            libJackSMS::dataTypes::configuredServicesType::iterator i=Esc.begin();
            libJackSMS::dataTypes::configuredServicesType::iterator i_end=Esc.end();
            for(;i!=i_end;++i){
                if (i.value().getId()==aid){

                    Esc.erase(i);
                    break;
                }

            }

            emit deleteOk();
        }else{
            emit deleteKo();
        }

    }catch(...){
        emit deleteKo();
    }


}
