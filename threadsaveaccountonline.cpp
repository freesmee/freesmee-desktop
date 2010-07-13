#include "threadsaveaccountonline.h"
#include "libjacksms/libJackSMS.h"
threadSaveAccountOnline::threadSaveAccountOnline(libJackSMS::dataTypes::configuredServicesType & _Esc,libJackSMS::dataTypes::servicesType & _S,QString _sessId,libJackSMS::dataTypes::configuredAccount &_account,const libJackSMS::dataTypes::optionsType &_opzioni):
        Esc(_Esc),S(_S),sessId(_sessId),account(_account),opzioni(_opzioni)
{
}

void threadSaveAccountOnline::run(){
    try{

        libJackSMS::serverApi::accountManager manager(sessId,opzioni);
        if (manager.addNewAccount(S[account.getService()],account)){
            QString id=manager.getResultId();
            account.setId(id);
            Esc.insert(id,account);
            emit saveOk();
        }else{

            emit saveKo();
        }
    }catch(...){
        emit this->saveKo();
    }
}
