/*
*/

#include "ConfiguredAccount.h"
#include "Exceptions.h"
#include "Utilities.h"
#include <map>

namespace libJackSMS{

    namespace dataTypes{

        /*********definitions for configuredAccount****************/
        configuredAccount::configuredAccount() :
                indiceStatistica(0),
                indiceDato(0),
                indiceOpzione(0){
        }

        void configuredAccount::setName(const QString &_value){
            nome=_value;
        }
        void configuredAccount::setId(const QString &_value){
            id=_value;
        }
        void configuredAccount::setService(const QString &_value){
            servizio=_value;
        }
        void configuredAccount::setStat(const QString &_statName,const QString &_statValue){
            statistiche.insert(_statName,_statValue);
        }
        void configuredAccount::setData(const QString &_dataName,const QString &_dataValue) {
            dati.insert(_dataName, _dataValue);
        }
        bool configuredAccount::updateData(const QString &_dataName,const QString &_dataValue){
            QMap<QString,QString>::const_iterator i=dati.find(_dataName);
            if (i==dati.end()){
                throw exceptionFieldNotFound(_dataName);
                return false;
            }else{
                dati[_dataName]=_dataValue;
            }
            return true;
        }
        void configuredAccount::setOption(const QString &_optName,const QString &_optValue){
            opzioni.insert(_optName,_optValue);
        }
        QString configuredAccount::getName() const{
            return nome;
        }
        QString configuredAccount::getId() const{
            return id;
        }
        QString configuredAccount::getService() const{
            return servizio;
        }
        QString configuredAccount::getStat(const QString &_statName) const{
            QMap<QString,QString>::const_iterator i=statistiche.find(_statName);
            if (i==statistiche.end())
                return "0";
            else
                return i.value();
        }
        bool configuredAccount::increaseStatValue(QString _varname){
            QString st=getStat(_varname);
            bool ok;
            int n=st.toInt(&ok,10)+1;
            statistiche[ _varname]=QString::number(n);
            return true;
        }
        QString configuredAccount::getData(const QString &_dataName) const{
            QMap<QString, QString>::const_iterator i = dati.find(_dataName);
            if (i == dati.end())
                return QString();
            else
                return i.value();
        }
        QString configuredAccount::getOption(const QString &_optName) const{
            QMap<QString,QString>::const_iterator i = opzioni.find(_optName);
            if (i==opzioni.end())
                return QString();
            else
                return i.value();
        }

        bool configuredAccount::nextData(){
            QMap<QString,QString>::const_iterator i_end=dati.end();
            if (indiceDato==0){
                iterDati=dati.begin();
            }else{
                ++iterDati;
            }
            ++indiceDato;
            if (iterDati==i_end){
                return false;
                indiceDato=0;
            }else
                return true;

        }
        /*ritorna il nome del dato corrente sotto all'iteratore*/
        QString configuredAccount::currentData() const{
            return iterDati.key();

        }
        bool configuredAccount::nextOption(){
            QMap<QString,QString>::const_iterator i_end=opzioni.end();
            if (indiceOpzione==0){
                iterOpzioni=opzioni.begin();
            }else{
                ++iterOpzioni;
            }
            ++indiceOpzione;
            if (iterOpzioni==i_end){
                return false;
                indiceOpzione=0;
            }else
                return true;

        }
        QString configuredAccount::currentOption() const{
            return iterOpzioni.key();

        }

        bool configuredAccount::nextStat(){
            QMap<QString,QString>::const_iterator i_end=statistiche.end();
            if (indiceStatistica==0){
                iterStatistiche=statistiche.begin();
            }else{
                ++iterStatistiche;
            }
            ++indiceStatistica;
            if (iterStatistiche==i_end) {
                return false;
                indiceStatistica=0;
            }else
                return true;

        }

        QString configuredAccount::currentStat() const{
            return iterStatistiche.key();
        }

        void configuredAccount::sumStats(configuredAccount other) {
            for (QMap<QString, QString>::iterator i = statistiche.begin(); i != statistiche.end(); ++i) {
                for (QMap<QString, QString>::const_iterator j = other.statistiche.begin(); j != other.statistiche.end(); ++j) {
                    if (i.key() == j.key())
                        i.value() = QString::number(i.value().toInt() + j.value().toInt());
                }
            }
        }

    }

}
