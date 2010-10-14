#include "LanguageManager.h"

#include <map>
#include <list>
#include <string>
#include <algorithm>
/*using singleton design pattern*/
namespace libJackSMS{
    namespace dataTypes{

        Language::Language(QString _lname){

            langName=_lname;

        }
        void Language::setString(const QString _id, const QString _string){
            coppie.insert(_id,_string);
        }
        QString Language::getString(const QString _id)const{
            QMap<QString,QString>::const_iterator i=coppie.find(_id);
            if (i==coppie.end())
                return "";
            else
                return i.value();


        }
        QString Language::getLangName() const{
            return langName;
        }


    }



    LanguageManager::LanguageManager():isSet(false),lan_iterator_counter(0){
        {
            dataTypes::Language l("Italiano");
        l.setString("1","Invia");
        l.setString("2","Annulla invio");
        l.setString("3","Accedi");
        l.setString("4","Non ho un account JackSMS");
        l.setString("5","Ho scordato la password");
        l.setString("6","Ricorda la password");
        l.setString("7","Accesso in corso");
        l.setString("8","Destinatario");
        l.setString("9","Servizio");
        l.setString("10","Rubrica");
        l.setString("11","Filtra");
        l.setString("12","Invia");
        l.setString("13","Ferma");
        l.setString("14","Conversazioni");
        l.setString("15","Rubrica");
        l.setString("16","Servizi");
        l.setString("17","Rispondi");
        l.setString("18","Inoltra");
        l.setString("19","Elimina selezionati");
        l.setString("20","Citazione");
        l.setString("21","Storico dei messaggi inviati e ricevuti con JackSMS");
        l.setString("22","Rubrica dei contatti");
        l.setString("23","Aggiungi");
        l.setString("24","Modifica");
        l.setString("25","Elimina");
        l.setString("26","Servizi d'invio SMS");
        l.setString("27","Stato ricezione:");
        l.setString("28","Attivo");
        l.setString("29","Connessione...");
        l.setString("30","Disattivo");
        l.setString("31","Login Automatico");
        //languages.push_back(l);
    }

        dataTypes::Language l("English");
        l.setString("1","Send");
        l.setString("2","Cancel send");
        l.setString("3","Login");
        l.setString("4","I haven't a JackSMS account");
        l.setString("5","I've lost my password");
        l.setString("6","Remember password");
        l.setString("7","Logging in");
        l.setString("8","Recipient");
        l.setString("9","Service");
        l.setString("10","Phone Book");
        l.setString("11","Search");
        l.setString("12","Send");
        l.setString("13","Stop");
        l.setString("14","Conversations");
        l.setString("15","Phone Book");
        l.setString("16","Services");
        l.setString("17","Reply");
        l.setString("18","Forward");
        l.setString("19","Delete selected");
        l.setString("20","Quote");
        l.setString("21","Sended and received with JackSMS");
        l.setString("22","Contacts phonebook");
        l.setString("23","Add");
        l.setString("24","Edit");
        l.setString("25","Delete");
        l.setString("26","Services");
        l.setString("27","Receiving status:");
        l.setString("28","Active");
        l.setString("29","Connecting...");
        l.setString("30","Inactive");
        l.setString("31","Automatic Login");

        //languages.push_back(l);

        /*
        dataTypes::Language l("Srpski");
        l.setString("1","Pošalji");
        l.setString("2","Odustani");
        l.setString("3","Login");
        l.setString("4","Nemam JackSMS nalog");
        l.setString("5","Izgubio sam šifru");
        l.setString("6","Zapamni šifru");
        l.setString("7","Ulogovavanje");
        l.setString("8","Primalac");
        l.setString("9","Servis");
        l.setString("10","Adresar");
        l.setString("11","Traži");
        l.setString("12","Pošalji");
        l.setString("13","Zaustavi");
        l.setString("14","Konverzacije");
        l.setString("15","Adresar");
        l.setString("16","Servisi");
        l.setString("17","Odgovori");
        l.setString("18","Prosledi");
        l.setString("19","Izbriši selektovano");
        l.setString("20","Citiraj");
        l.setString("21","Poslalo i primljeno pomocu JackSMS");
        l.setString("22","Adresar kontakata");
        l.setString("23","Dodaj");
        l.setString("24","Ažuriraj");
        l.setString("25","Izbriši");
        l.setString("26","Servisi");
        l.setString("27","Status slanja:");
        l.setString("28","Aktivan");
        l.setString("29","Konektovanje...");
        l.setString("30","Neaktivan");
        l.setString("31","Automatski login");
        //languages.push_back(l);
        */



    }
    LanguageManager* LanguageManager::istance=NULL;
    LanguageManager* LanguageManager::getIstance(){
        if (istance==NULL){
            istance=new LanguageManager();
        }
        return istance;
    }
    QString LanguageManager::getString(const QString &_id){
        return currentLanguage->getString(_id);
    }
    //bool LanguageManager::isSet()const{
    //    return isSet;
    //}
    bool LanguageManager::nextLanguage(){
        QList<dataTypes::Language>::const_iterator i_end=languages.end();
        if (lan_iterator_counter==0){
            lanIter=languages.begin();
        }else{
            ++lanIter;
        }
        ++lan_iterator_counter;
        if (lanIter==i_end){

            lan_iterator_counter=0;
            return false;
        }else
            return true;

    }
    /*ritorna il nome del dato corrente sotto all'iteratore*/
    QString LanguageManager::currentLang() const{
        return lanIter->getLangName();

    }
    bool LanguageManager::setLanguage(QString _name){
        _name=_name.toUpper();
        bool setted=false;
        QList<dataTypes::Language>::const_iterator i=languages.begin();
        QList<dataTypes::Language>::const_iterator i_end=languages.end();
        for(;i!=i_end;++i){
            if (i->getLangName().toUpper()==_name){
                currentLanguage=i;
                setted=true;
                break;
            }

        }
        isSet=setted;
        return setted;
    }


}
