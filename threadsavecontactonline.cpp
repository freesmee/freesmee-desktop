#include "threadsavecontactonline.h"
#include "libjacksms/libJackSMS.h"
#include "Types.h"
threadSaveContactOnline::threadSaveContactOnline(libJackSMS::dataTypes::phoneBookType & _rubrica,QString _sessId,libJackSMS::dataTypes::contact & _contatto,const libJackSMS::dataTypes::optionsType &_opzioni):
        Rubrica(_rubrica),sessId(_sessId),contatto(_contatto),opzioni(_opzioni)
{
}

void threadSaveContactOnline::run(){
    try{

        libJackSMS::serverApi::contactManager manager(sessId,opzioni);
        if (manager.addNewContact(contatto)){
            QString id=manager.getResultId();
            QString nome=contatto.getName();
            QString gruppo=contatto.getGroup();
            QString account=contatto.getAccount();
            libJackSMS::dataTypes::contact con(nome,contatto.getPhone(),gruppo,account);
            //con(nome,contatto.getPhone(),gruppo,account);
            con.setId(id);
            Rubrica.insert(id,con);
            emit saveOk();
        }else{

            emit saveKo();

        }
    }catch(...){
        emit this->saveKo();
    }
}
