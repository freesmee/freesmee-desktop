#include "ProxyConfig.h"
#include <QString>

namespace libJackSMS{
    namespace dataTypes{

        proxySettings::proxySettings():
            usaproxy(false),autenticati(false){


        }

        proxySettings::proxySettings(const dataTypes::optionsType &_opzioni):
            usaproxy(false),autenticati(false)

        {
            usaproxy=false;
            dataTypes::optionsType::const_iterator iter=_opzioni.find("use-proxy");
             if (iter!=_opzioni.end()){
                 if (iter.value()=="yes"){
                     usaproxy=true;
                     iter=_opzioni.find("proxy");
                     if (iter!=_opzioni.end()){
                        server=iter.value();
                     }
                     iter=_opzioni.find("proxy-port");
                     if (iter!=_opzioni.end()){
                        port=iter.value();
                     }
                     iter=_opzioni.find("proxy-type");
                     if (iter!=_opzioni.end()){
                        type=iter.value();
                     }
                     iter=_opzioni.find("proxy-username");
                     if (iter!=_opzioni.end()){
                         username=iter.value();
                     }
                     iter=_opzioni.find("proxy-password");
                     if (iter!=_opzioni.end()){
                        password=iter.value();
                     }
                     iter=_opzioni.find("use-proxy-auth");
                     if (iter!=_opzioni.end()){
                        if (iter.value()=="yes")
                            autenticati=true;
                     }


                 }
             }
        }
        QString proxySettings::getServer() const{
            return server;
        }
        QString proxySettings::getPort() const{
            return port;
        }
        QString proxySettings::getType() const{
            return type;
        }
        QString proxySettings::getUsername() const{
            return username;
        }
        QString proxySettings::getPassword() const{
            return password;
        }
        bool proxySettings::useProxy() const{
            return usaproxy;
        }
        bool proxySettings::useAuth() const{
            return autenticati;
        }


    }


}
