#include "threadupdatecontact.h"
#include "libjacksms/libJackSMS.h"

threadUpdateContact::threadUpdateContact(libJackSMS::dataTypes::phoneBookType & _rubrica,QString _sessId,libJackSMS::dataTypes::contact & _contatto,const libJackSMS::dataTypes::optionsType &_opzioni):
        Rubrica(_rubrica),sessId(_sessId),contatto(_contatto),opzioni(_opzioni)
{
}

void threadUpdateContact::run(){
    try{

        libJackSMS::serverApi::contactManager manager(sessId,opzioni);
        if (manager.updateContact(contatto)){
            Rubrica[contatto.getId()]=contatto;
            emit updateOk();
        }else{

            emit updateKo();

        }
    }catch(...){
        emit updateKo();
    }
}
