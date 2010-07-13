#include "threadsendsms.h"
#include "sendsmsoperations.h"

threadsendsms::threadsendsms(const libJackSMS::dataTypes::optionsType & _opzioni,libJackSMS::reportOperationSendSms * _op,const libJackSMS::dataTypes::configuredAccount &_Account,const libJackSMS::dataTypes::servicesType &_ElencoServizi,libJackSMS::dataTypes::shortMessage _msg,libJackSMS::dataTypes::phoneNumber _recipient):opzioni(_opzioni),op(_op),Account(_Account),ElencoServizi(_ElencoServizi),msg(_msg),recipient(_recipient)
{

}
void threadsendsms::abort(){
    //sndr->abort();
}
void threadsendsms::cancelSend(){
    emit signalAbort();
}

void threadsendsms::run(){

    try{
        libJackSMS::smsSender sndr(ElencoServizi,opzioni);
        connect(this,SIGNAL(signalAbort()),&sndr,SLOT(abort()));
        sndr.setMessage(msg);
        sndr.setRecipient(recipient);
        sndr.setAccount(Account);
        sndr.setReportOperation(op);
        sndr.send();
    }catch(libJackSMS::netClient::abortedException e){
        emit this->fatalError();

    }catch(libJackSMS::exceptionXmlError e){
        emit this->fatalError();
    }
}


