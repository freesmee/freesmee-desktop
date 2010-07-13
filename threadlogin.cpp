#include "threadlogin.h"
#include "sendsmsoperations.h"
#include <QMessageBox>
#include "libjacksms/libJackSMS.h"

 void ThreadLogin::run()
 {
     try{

        if (signin->doLogin()){
            libJackSMS::localApi::userDirectoryManager man(userName);
            if (!man.userDirectoryExists()){
                man.createUser();
                userDirectory=man.getUserDir();
            }


            libJackSMS::localApi::xmlLoader loader(userDirectory);
            loader.loadServices(ElencoServizi);
            loader.loadImLog(ArchivioIm);
            loader.loadSmsLog(ArchivioMessaggi);
            loader.loadOptions(Opzioni,false);

            signin->retreiveAccounts(ElencoServiziConfigurati);
            libJackSMS::serverApi::synchronizeVariables(ElencoServiziConfigurati,ElencoServizi);
            libJackSMS::localApi::statsManager manag(userDirectory);
            manag.loadStats(ElencoServiziConfigurati);
            signin->retreivePhoneBook(Rubrica);



           /*{
               libJackSMS::dataTypes::phoneBookType::const_iterator i=cRubrica.begin();
               libJackSMS::dataTypes::phoneBookType::const_iterator i_end=cRubrica.end();
               for(;i!=i_end;++i){
                   QString nome=QString::fromUtf8(i.value().getName().c_str(),i.value().getName().length());
                   QString gruppo=QString::fromStdString(i.value().getGroup());
                   QString account=QString::fromStdString(i.value().getAccount());

                   QString id=QString::fromStdString(i.value().getId());
                   QContact con(nome,i.value().getPhone(),gruppo,account);
                   con.setId(id);
                   rubrica.insert(id,con);
               }
           }*/


            {
               libJackSMS::dataTypes::logImType::const_iterator i=ArchivioIm.begin();
                libJackSMS::dataTypes::logImType::const_iterator i_end=ArchivioIm.end();


                for(;i!=i_end;++i){
                    QMyMessage msg;
                    msg.setAccountId(QString::number(0));
                    QDateTime data=i.value().getDate().getData();
                    /*if (!data.isValid()){
                        data=QDateTime::fromString(QString::fromStdString(i.value().getDate().getStringDay())+" "+QString::fromStdString(i.value().getDate().getStringHour()),"d/M/yy HH:mm:ss");
                    }*/
                    msg.setData(data);
                    msg.setMessage(i.value().getText());
                    //msg.setMessage(QString::fromStdString(i.value().getText()));
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
                    /*if (!data.isValid()){
                        data=QDateTime::fromString(QString::fromStdString(i.value().getDate().getStringDay())+" "+QString::fromStdString(i.value().getDate().getStringHour()),"d/M/yy HH:mm:ss");
                    }*/
                    msg.setData(data);
                    msg.setMessage(i.value().getText());
                    //msg.setMessage(QString::fromStdString(i.value().getText()));
                    msg.setId(i.value().getId());
                    msg.setIsReceived(false);
                    msg.setPhone(i.value().getPhoneNumber());
                    msg.setServiceId(i.value().getServiceId());
                    messaggi.push_back(msg);
                }
            }


            result=true;

        }else{
            error=signin->getLoginError();

            result=false;
        }
    }catch(libJackSMS::exceptionXmlError e){
        error=e.what();
        result=false;
    }catch(libJackSMS::exceptionXmlNotFound e){
        error=e.what();
        result=false;
    }catch(libJackSMS::exceptionSomethingWrong e){
        error=e.what();
        result=false;
    }catch(libJackSMS::exceptionPhoneNumber e){
        error=QString("phone number exception");
        result=false;

    }catch(...){
        error=QString("Errore sconosciuto");
        result=false;
    }

 }
 ThreadLogin::ThreadLogin(libJackSMS::serverApi::login * _signin,libJackSMS::dataTypes::servicesType & _ElencoServizi,libJackSMS::dataTypes::optionsType &_Opzioni,libJackSMS::dataTypes::configuredServicesType &_ElencoServiziConfigurati,libJackSMS::dataTypes::phoneBookType &_rubrica, QString &_userDirectory,QString _userName,types::QMessageListType &_messaggi)
         :signin(_signin),ElencoServizi(_ElencoServizi),Opzioni(_Opzioni),ElencoServiziConfigurati(_ElencoServiziConfigurati),Rubrica(_rubrica),userDirectory(_userDirectory),userName(_userName),messaggi(_messaggi){



 }
 ThreadLogin::~ThreadLogin(){
 }
 QString ThreadLogin::getError()
 {

        return error;

 }
