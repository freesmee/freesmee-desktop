#include "messageloader.h"
#include "libjacksms/libJackSMS.h"
messageLoader::messageLoader(QString _ud):ud(_ud),aborted(false)
{
    qRegisterMetaType<QList<QMyMessage> >("QList<QMyMessage>");
}

void messageLoader::run(){
        QList<QMyMessage> messaggi;
        libJackSMS::localApi::xmlLoader loader(ud);
        libJackSMS::dataTypes::logImType ArchivioIm;
        libJackSMS::dataTypes::logSmsType ArchivioMessaggi;
        loader.loadImLog(ArchivioIm);
        loader.loadSmsLog(ArchivioMessaggi);



        {
           libJackSMS::dataTypes::logImType::const_iterator i=ArchivioIm.begin();
           libJackSMS::dataTypes::logImType::const_iterator i_end=ArchivioIm.end();


            for(;i!=i_end;++i){
                QMyMessage msg;
                msg.setAccountId(QString::number(0));
                QDateTime data=i.value().getDate().getData();
                msg.setData(data);
                msg.setMessage(i.value().getText());

                msg.setId(i.value().getId());
                msg.setIsReceived(true);
                msg.setPhone(i.value().getPhoneNumber());
                messaggi.push_back(msg);
            }
        }
        {
            libJackSMS::dataTypes::logSmsType::const_iterator i=ArchivioMessaggi.begin();
            libJackSMS::dataTypes::logSmsType::const_iterator i_end=ArchivioMessaggi.end();
            for(;i!=i_end;++i){
                QMyMessage msg;
                msg.setAccountName(i.value().getAccount());
                QDateTime data=i.value().getDate().getData();
                msg.setData(data);
                msg.setMessage(i.value().getText());
                msg.setId(i.value().getId());
                msg.setIsReceived(false);
                msg.setPhone(i.value().getPhoneNumber());
                msg.setServiceId(i.value().getServiceId());
                messaggi.push_back(msg);
            }
        }
        if (!aborted)
            emit messagesLoaded(messaggi);

}

void messageLoader::loadMessages(){
    start();

}
void messageLoader::abort(){
    aborted=true;

}
