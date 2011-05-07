/*
    Copyright (C) <2011>

    <enrico bacis> <enrico.bacis@gmail.com>
    <ivan vaccari> <grisson@jacksms.it>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    You can't modify the adv system, to cheat it.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef SERVICE_HH
#define SERVICE_HH 1

#include "PaginaServizio.h"
#include "VariabileServizio.h"
#include "ConfiguredAccount.h"
#include <QString>
#include <QList>
#include <QMap>
#include <QIcon>
#include "Encoding.h"

namespace libJackSMS
{
    namespace dataTypes
    {
        class service
        {
        private:
            QString nome;
            QString id;
            QString versione;
            QString smsMassimi;
            QString codifica;
            QString lunghezzaMassima;
            QString lunghezzaSingola;
            QString periodoReset;
            QString lingua;
            bool mantieniSessione;
            QString urlSessione;
            QString descrizione;
            QImage icona;
            QList<variabileServizio> variabili;
            int varCounter;
            QList<variabileServizio>::iterator indexVar;
            //QMap<QString,QString> statistiche;
            QMap<QString,QString> dati;
            QMap<QString,QString> opzioni;
            QList<paginaServizio> pagine;
            paginaServizio postprocedurePage;
            int numberOfPages;
            int pageCounter;
            QList<paginaServizio>::iterator indexPage;
            libJackSMS::encodingGeneric *textEncoder;
            QString smsDivisor;

        public:
            service();
            //void init(const dataTypes::configuredAccount & _account);
            void setName(QString _name);
            QString getName() const;
            void setId(QString _id);
            QString getId() const;
            void setVersion(QString _version);
            QString getVersion() const;
            void setMaxSms(QString _maxSms);
            QString getMaxSms() const;
            void setEncoding(QString _encoding);
            QString getEncoding() const;
            void setMaxLength(QString _maxLen);
            QString getMaxLength() const;
            int getIntMaxLength() const;
            void setSingleLength(QString _singleLen);
            QString getSingleLength() const;
            int getIntSingleLength() const;
            void setReset(QString _reset);
            QString getReset() const;
            void setIcon(const QImage &_icon);
            QIcon getIcon() const;
            void setLanguage(QString _lan);
            QString getLanguage() const;
            void setMantainSession(bool _mantain);
            bool getMantainSession() const;
            void setSessionUrl(QString _sessionUrl);
            QString getSessionUrl() const;
            void setDescription(QString _description);
            QString getDescription() const;
            void setPage(const dataTypes::paginaServizio &_pag);
            void setPostprocedurePage(const dataTypes::paginaServizio &_pag);
            bool getHasPostprocedurePage() const;

            /*variabili richieste dal servizio, username ecc..*/
            void setVariable(const variabileServizio & _var);
            dataTypes::variabileServizio getVariable(const QString & _varName) const ;
            bool nextVar();
            dataTypes::variabileServizio & currentVar();
            //void setStat(const QString & _statName,const QString & _statValue);
            //QString getStat(const QString & _varName) const;
            /*dati del servizio riferiti all'account corrente*/
            void setData(const QString & _dataName,const QString & _dataValue);
            QString getData(const QString & _dataName) const ;
            /*opzioni del servizio riferite all'account corrente*/
            void setOption(const QString & _optionName,const QString & _optionValue);
            QString getOption(const QString & _optionName) const;
            bool nextPage();
            dataTypes::paginaServizio &currentPage();
            dataTypes::paginaServizio &getPostprocedurePage();
            bool hasPostprocedurePage;
            QString getEncodedText(const QString & _text);
            QString getEncodedTextUrlEncoded(const QString & _text);
            int getNumberOfPages() const;
            void setSmsDivisor(QString _div);
            QString getSmsDivisor() const;
            int getIntSmsDivisor() const;
            encodingGeneric * getTextEncoder() const;

        };

    }
}

#endif //SERVICE_HH
