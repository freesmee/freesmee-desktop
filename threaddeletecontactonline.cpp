#include "threaddeletecontactonline.h"
#include "Types.h"

threadDeleteContactOnline::threadDeleteContactOnline(libJackSMS::dataTypes::phoneBookType & _rubrica,QString _sessId,QString _contactId,const libJackSMS::dataTypes::optionsType &_opzioni):
        Rubrica(_rubrica),sessId(_sessId),contactId(_contactId),opzioni(_opzioni)
{
}

void threadDeleteContactOnline::run(){
    try{

        libJackSMS::serverApi::contactManager manager(sessId,opzioni);
        if (manager.deleteContact(contactId)){
            libJackSMS::dataTypes::phoneBookType::iterator i=Rubrica.begin();
            libJackSMS::dataTypes::phoneBookType::iterator i_end=Rubrica.end();
            for(;i!=i_end;++i){
                if (i->getId()==contactId){

                    Rubrica.erase(i);
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
