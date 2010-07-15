#include "threadsavesmsonline.h"
#include <QString>
threadSaveSmsOnline::threadSaveSmsOnline(QString _loginId,libJackSMS::dataTypes::logSmsMessage _msg,const libJackSMS::dataTypes::optionsType &_opzioni,QString _userDir,QString _accountId):
        loginId(_loginId),msg(_msg),opzioni(_opzioni),userDir(_userDir),accountId(_accountId)
{

}
void threadSaveSmsOnline::run(){

    try{

        libJackSMS::serverApi::smsLogSaver saver(loginId,opzioni);
        saver.setMessage(msg);
        QString _id;

        if(saver.save(_id)){
            msg.setId(_id);
            std::cout<<"[SAVE] messaggio sms salvato online con id "<<_id.toStdString()<<std::endl;

            emit smsSaved();
        }else{
            std::cout<<"[SAVE] fallito salvataggio sms online "<<_id.toStdString()<<std::endl;
            emit smsNotSaved();
        }

    }catch(libJackSMS::exceptionXmlError e){
        emit this->fatalError();
    }catch(...){
        emit this->fatalError();
    }

}

