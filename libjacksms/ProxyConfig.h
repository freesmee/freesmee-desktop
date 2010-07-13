
#ifndef PROXYCONFIG_H
#define PROXYCONFIG_H
#include <QString>
#include "DataTypes.h"

namespace libJackSMS{
    namespace dataTypes{
        /**
            Estrae dalle opzioni i dati necessari a configurare il proxy e crea una struttura ad hoc

        */

        class proxySettings{
        private:
            QString server;
            QString port;
            QString type;
            QString username;
            QString password;
            bool usaproxy;
            bool autenticati;
        public:
            proxySettings();
            proxySettings(const dataTypes::optionsType &_opzioni);
            QString getServer() const;
            QString getPort() const;
            QString getType() const;
            QString getUsername() const;
            QString getPassword() const;
            bool useProxy() const;
            bool useAuth() const;
        };

    }


}
#endif // PROXYCONFIG_H
