#include "ProxyConfig.h"
#include <QString>
#include <QDebug>

namespace libJackSMS {

    namespace dataTypes {

        proxySettings::proxySettings() :
                usaproxy(false),
                autenticati(false)
        {
        }

        proxySettings::proxySettings(const dataTypes::optionsType &_opzioni) :
                usaproxy(false),
                autenticati(false)
        {
            usaproxy = false;

            if (_opzioni["use-proxy"] == "yes") {

                usaproxy = true;
                dataTypes::optionsType::const_iterator iter;

                iter = _opzioni.find("proxy");
                if (iter != _opzioni.end())
                    server = iter.value();

                iter = _opzioni.find("proxy-port");
                if (iter != _opzioni.end())
                    port = iter.value();

                iter = _opzioni.find("proxy-type");
                if (iter != _opzioni.end())
                    type = iter.value();

                iter = _opzioni.find("proxy-username");
                if (iter != _opzioni.end())
                    username = iter.value();

                iter = _opzioni.find("proxy-password");
                if (iter != _opzioni.end())
                    password = iter.value();

                if (_opzioni["use-proxy-auth"] == "yes")
                    autenticati = true;
            }
        }

        QString proxySettings::getServer() const {
            return server;
        }

        QString proxySettings::getPort() const {
            return port;
        }

        QString proxySettings::getType() const {
            return type;
        }

        QString proxySettings::getUsername() const {
            return username;
        }

        QString proxySettings::getPassword() const {
            return password;
        }

        bool proxySettings::useProxy() const {
            return usaproxy;
        }

        bool proxySettings::useAuth() const {
            return autenticati;
        }
    }

}
