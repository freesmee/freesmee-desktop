
/*
    Copyright (C) <2009>  <ivan vaccari> <grisson@jacksms.it>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "DataTypes.h"
#include "DataTypesApi.h"
#ifndef XMLPARSERSERVERAPI_HH_GUARD
#define XMLPARSERSERVERAPI_HH_GUARD 1




namespace libJackSMS{

    namespace xmlParserApi{
        class xmlParserServerApiGeneric{
            public:
                xmlParserServerApiGeneric(){};
                virtual ~xmlParserServerApiGeneric(){};
                virtual void setXml(QString _xml)=0;
                virtual bool checkIsLogged()=0;
                virtual QString getSessionId()=0;
                virtual QString getError()=0;
                virtual bool loadErrors(dataTypesApi::errorsType & _errors)=0;
                virtual bool loadPhoneBook(libJackSMS::dataTypes::phoneBookType & _rubrica)=0;
                virtual bool loadPhoneBook2(libJackSMS::dataTypes::phoneBookType & _rubrica)=0;
                virtual bool loadAccounts(libJackSMS::dataTypes::configuredServicesType & _serviziConfigurati)=0;

                /**
                 * Estrae dal codice XML ricevuto dal server i messaggi IM
                 * \return \b true se almeno un messaggio e' presente, false altrimenti
                 */
                virtual bool extractImMessages(libJackSMS::dataTypes::logImType & _logIm)=0;

                /**
                 * Controlla se l'xml di risposta dell'api di salvataggio sms online contiene il valore di successo o meno
                 * \return \b true se il messaggio e' stato salvato, false altrimenti
                 */
                virtual bool checkSaved(QString &_id, QString &totalForAccount)=0;

                /**
                 * Controlla se l'xml di risposta dell'api di aggiunta nuovo contatto online contiene il valore di successo o meno
                 * \param \b _resId  la variabile dove verrà messo l'id del nuovo contatto
                 * \return \b true se il contatto e' stato salvato, false altrimenti
                 */
                virtual bool checkAddNewContact(QString &_resId,bool & canReceiveJms)=0;

                /**
                 * Controlla se l'xml di risposta dell'api di aggiornamento contatto online contiene il valore di successo o meno
                 * \return \b true se il contatto e' stato aggiornato, false altrimenti
                 */
                virtual bool checkUpdateContact()=0;

                /**
                 * Controlla se l'xml di risposta dell'api di eliminazione di un contatto online contiene il valore di successo o meno
                 * \return \b true se il contatto e' stato eliminato, false altrimenti
                 */
                virtual bool checkDeleteContact()=0;
                /**
                 * Controlla se l'xml di risposta dell'api di aggiunta nuovo account online contiene il valore di successo o meno
                 * \return \b true se l'account e' stato salvato, false altrimenti
                 */
                virtual bool checkAddNewAccount(QString &_resId)=0;

                /**
                 * Controlla se l'xml di risposta dell'api di aggiornamento di un account online contiene il valore di successo o meno
                 * \return \b true se l'account e' stato aggiornato, false altrimenti
                 */
                virtual bool checkUpdateAccount()=0;

                /**
                 * Controlla se l'xml di risposta dell'api di eliminazione di un account online contiene il valore di successo o meno
                 * \return \b true se l'account e' stato eliminato, false altrimenti
                 */
                virtual bool checkDeleteAccount()=0;
                /**
                 * Parsa l'xml delle api della conversazione e ne estrae i messaggi
                 * \return true se almeno un messaggio e' presente, false altrimenti
                 */
                virtual bool parseConversation(libJackSMS::dataTypes::logSmsType & _logSms,libJackSMS::dataTypes::logImType & _logIm)=0;
                /**
                 * Parsa l'xml delle api durante l'aggiornamento dei servizi e costruisce un elenco di servizi nuovo.
                 * \return true se non ho avuto errori, false altrimenti
                 */
                virtual bool parseServices(libJackSMS::dataTypes::servicesType &_servizi)=0;
                virtual bool checkReport()=0;
                virtual QString getVersion()=0;

        };

    }
}
#endif //XMLPARSERSERVERAPI_HH_GUARD

