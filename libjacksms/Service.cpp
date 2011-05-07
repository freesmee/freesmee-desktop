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

#include "Service.h"
#include "DataTypes.h"
#include "EncodingUrl.h"
#include "EncodingUtf8.h"
#include "EncodingNull.h"
#include "Encoding.h"
#include "EncodingRemoveAccents.h"
#include "Utilities.h"
#include "EncodingIso88591.h"
#include <QIcon>

namespace libJackSMS {

    namespace dataTypes {

        service::service() {
            mantieniSessione = false;
            codifica = "URL";
            pageCounter = 0;
            varCounter = 0;
            textEncoder = new libJackSMS::encodingUrl();
            numberOfPages = 0;
            hasPostprocedurePage = false;
        }

        void service::setName(QString _name) {
            nome = _name;
        }

        QString service::getName() const {
            return nome;
        }

        void service::setId(QString _id) {
            id = _id;
        }

        QString service::getId() const {
            return id;
        }

        void service::setVersion(QString _version) {
            versione = _version;
        }

        QString service::getVersion() const {
            return versione;
        }

        void service::setMaxSms(QString _maxSms) {
            smsMassimi = _maxSms;
        }

        QString service::getMaxSms() const {
            return smsMassimi;
        }

        void service::setEncoding(QString _encoding) {
            _encoding = _encoding.toUpper();

            if (_encoding == "UTF-8") {
                textEncoder->~encodingGeneric();
                textEncoder = new libJackSMS::encodingUtf8();
                codifica = _encoding;
            } else if (_encoding == "ACCENT") {
                textEncoder->~encodingGeneric();
                textEncoder = new libJackSMS::encodingRemoveAccents();
                codifica = _encoding;
            } else if (_encoding == "URL") {
                textEncoder->~encodingGeneric();
                textEncoder = new libJackSMS::encodingUrl();
                codifica = _encoding;
            } else if (_encoding == "ISO-8859-1") {
                textEncoder->~encodingGeneric();
                textEncoder = new libJackSMS::encodingISO88591();
                codifica = _encoding;
            }
        }

        QString service::getEncoding() const {
            return codifica;
        }

        void service::setSingleLength(QString _singleLen) {
            lunghezzaSingola = _singleLen;
        }

        QString service::getSingleLength() const {
            return lunghezzaSingola;
        }

        void service::setIcon(const QImage &_icon) {
            icona = _icon;
        }

        QIcon service::getIcon() const {
            return QIcon(QPixmap::fromImage(icona));
        }

        /***/
        void service::setSmsDivisor(QString _div) {
            smsDivisor = _div;
        }

        QString service::getSmsDivisor() const {
            return smsDivisor;
        }

        int service::getIntSmsDivisor() const {
            return smsDivisor.toInt();
        }
        /***/

        void service::setMaxLength(QString _maxLen) {
            lunghezzaMassima = _maxLen;
        }

        QString service::getMaxLength() const {
            return lunghezzaMassima;
        }

        int service::getIntMaxLength() const {
            return lunghezzaMassima.toInt();
        }

        int service::getIntSingleLength() const {
            return lunghezzaSingola.toInt();
        }

        void service::setReset(QString _reset) {
            periodoReset = _reset;
        }

        QString service::getReset() const {
            return periodoReset;
        }

        void service::setLanguage(QString _lan) {
            lingua = _lan;
        }

        QString service::getLanguage() const {
            return lingua;
        }

        void service::setMantainSession(bool _mantain) {
            mantieniSessione = _mantain;
        }

        bool service::getMantainSession() const {
            return mantieniSessione;
        }

        void service::setSessionUrl(QString _sessionUrl) {
            urlSessione = _sessionUrl;
        }

        QString service::getSessionUrl() const {
            return urlSessione;
        }

        void service::setDescription(QString _description) {
            descrizione = _description;
        }

        QString service::getDescription() const {
            return descrizione;
        }

        void service::setVariable(const variabileServizio &_var) {
            variabili.push_back(_var);
        }

        variabileServizio service::getVariable(const QString &_varName) const {
            for (QList<variabileServizio>::const_iterator i = variabili.begin(); i != variabili.end(); ++i) {
                if (i->getName() == _varName) {
                    return *i;
                    break;
                }
            }
            return variabileServizio();
        }

        /*
        void service::setStat(const QString &_statName, const QString &_statValue) {
            statistiche.insert(std::make_pair(_statName, _statValue));
        }

        QString service::getStat(const QString &_varName) const {
            QMap<QString, QString>::const_iterator i = statistiche.find(_varName);
            if (i == statistiche.end())
                return "";
            else
                return i->second;
        }
        */

        void service::setData(const QString &_dataName, const QString &_dataValue) {
            dati.insert(_dataName, _dataValue);
        }

        QString service::getData(const QString &_dataName) const {
            QMap<QString, QString>::const_iterator i = dati.find(_dataName);
            if (i == dati.end())
                return "";
            else
                return i.value();
        }

        void service::setOption(const QString &_optionName, const QString &_optionValue) {
            opzioni.insert(_optionName, _optionValue);
        }

        QString service::getOption(const QString &_optionName) const {
            QMap<QString, QString>::const_iterator i = opzioni.find(_optionName);
            if (i == opzioni.end())
                return "";
            else
                return i.value();
        }

        bool service::nextPage() {
            //std::cout<<pagine.size();
            QList<paginaServizio>::const_iterator i_end = pagine.end();
            if (pageCounter == 0) {
                indexPage = pagine.begin();
            } else {
                ++indexPage;
            }

            ++pageCounter;

            if (indexPage == i_end) {
                pageCounter = 0;
                return false;
            } else
                return true;
        }

        paginaServizio &service::currentPage() {
            return *indexPage;
        }

        paginaServizio &service::getPostprocedurePage() {
            return postprocedurePage;
        }

        bool service::nextVar()
        {
            QList<variabileServizio>::const_iterator i_end = variabili.end();
            if (varCounter == 0)
                indexVar = variabili.begin();
            else
                ++indexVar;

            ++varCounter;
            if (indexVar == i_end) {
                varCounter = 0;
                return false;
            } else {
                return true;
            }
        }

        variabileServizio & service::currentVar() {
            return *indexVar;
        }

        libJackSMS::encodingGeneric * service::getTextEncoder() const {
            return textEncoder;
        }

        QString service::getEncodedText(const QString &_text) {
            return textEncoder->getEncodedString(_text);
        }

        QString service::getEncodedTextUrlEncoded(const QString &_text) {
            return textEncoder->getEncodedAndUrlString(_text);
        }

        void service::setPage(const dataTypes::paginaServizio &_pag) {
            pagine.push_back(_pag);
            numberOfPages++;
        }

        void service::setPostprocedurePage(const dataTypes::paginaServizio &_pag) {
            postprocedurePage = _pag;
            hasPostprocedurePage = true;
        }

        bool service::getHasPostprocedurePage() const {
            return hasPostprocedurePage;
        }

        int service::getNumberOfPages() const{
            return numberOfPages;
        }
    }
}
