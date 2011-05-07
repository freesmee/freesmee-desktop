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

#include "PaginaServizio.h"
#include <QList>

namespace libJackSMS{
    namespace dataTypes{
        paginaServizio::paginaServizio() :
                captcha(false),
                conter_iter_headers(0),
                conter_iter_variables(0),
                counter_iter_contents(0),
                ha_condizione(false),
                sleepbefore(0),
                counter_iter_commands(0)
        {
        }

        void paginaServizio::setUrl(const QString &_url) {
            url = _url;
        }

        QString paginaServizio::getUrl() const {
            return url;
        }

        void paginaServizio::setMethod(const QString &_method) {
            metodo = _method.toUpper();
        }

        QString paginaServizio::getMethod() const {
            return metodo;
        }

        void paginaServizio::setIsCaptcha(bool _iscaptcha) {
            captcha = true;
        }

        bool paginaServizio::getIsCaptcha() const {
            return captcha;
        }

        void paginaServizio::setSleepbefore(int _sleepbefore) {
            if (_sleepbefore < 0)
                sleepbefore = 0;
            else if (_sleepbefore > 60)
                sleepbefore = 60;
            else
                sleepbefore = _sleepbefore;
        }

        int paginaServizio::getSleepbefore() const {
            return sleepbefore;
        }

        void paginaServizio::setError(const pageError & _error) {
            errori.push_back(_error);
        }

        void paginaServizio::setRetreivedHtml(const QString & _html) {
            pageHtml = _html;
        }

        bool paginaServizio::checkErrors() {
            for (QList<pageError>::const_iterator i = errori.begin(); i != errori.end(); ++i) {

                if (pageHtml.indexOf(i->getErrorString()) != -1) {
                    currentError = *i;
                    return true;
                }
            }
            return false;
        }

        pageError paginaServizio::getError() {
            return currentError;
        }

        bool paginaServizio::checkAccepts() {
            for(QList<pageAccept>::const_iterator i = accettanti.begin(); i != accettanti.end(); ++i) {
                if (pageHtml.indexOf(i->getAcceptString()) != -1) {
                    currentAccept = *i;
                    return true;
                }
            }
            return false;
        }

        pageAccept paginaServizio::getAccept() {
            return currentAccept;
        }

        void paginaServizio::setCondition(const QString &_condizione) {
            condizione = _condizione;
            ha_condizione = true;
        }

        bool paginaServizio::hasCondition() const {
            return ha_condizione;
        }

        QString paginaServizio::getCondition()const {
            return condizione;
        }

        void paginaServizio::setVariable(const pageVariable &_var) {
            variabili.push_back(_var);
        }

        void paginaServizio::setContent(const pageContent &_con) {
            contenuti.push_back(_con);
        }

        void paginaServizio::setCommand(const pageCommand &_cmd) {
            comandi.push_back(_cmd);
        }

        void paginaServizio::setAccept(const pageAccept &_acc) {
            accettanti.push_back(_acc);
        }

        void paginaServizio::setHeader(const pageHeader &_header) {
            intestazioni.push_back(_header);
        }

        bool paginaServizio::nextHeader() {
            //std::cout<<pagine.size();
            QList<pageHeader>::const_iterator i_end = intestazioni.end();
            if (conter_iter_headers == 0) {
                iter_headers=intestazioni.begin();
            } else {
                ++iter_headers;
            }

            ++conter_iter_headers;
            if (iter_headers == i_end) {
                conter_iter_headers = 0;
                return false;
            } else
                return true;
        }

        bool paginaServizio::nextContent() {
            //std::cout<<pagine.size();
            QList<pageContent>::const_iterator i_end = contenuti.end();
            if (counter_iter_contents == 0) {
                iter_contents = contenuti.begin();
            } else {
                ++iter_contents;
            }

            ++counter_iter_contents;
            if (iter_contents == i_end) {
                counter_iter_contents = 0;
                return false;
            } else
                return true;
        }

        QString paginaServizio::currentContentName() const {
            return iter_contents->getName();
        }

        QString paginaServizio::currentContentValue() const {
            //if (!iter_contents->getToEncode())
            return pageHtml.mid(currentContentPosition.first, currentContentPosition.second - currentContentPosition.first);
            //else
            //    return QString(pageHtml.mid(currentContentPosition.first,currentContentPosition.second-currentContentPosition.first).toAscii().toPercentEncoding());
        }

        /*bool paginaServizio::currentContentToEncode() const {
            return iter_contents->getToEncode();
        }*/

        bool paginaServizio::currentContentFound() {
            QString l = iter_contents->getLeft();
            QString r = iter_contents->getRight();
            int _posL = pageHtml.indexOf(l);

            if (_posL == -1) {
                currentContentPosition = qMakePair(_posL, _posL);
                return false;
            } else {
                int _posR = pageHtml.indexOf(r, _posL + l.length());

                if (_posR == -1) {
                    currentContentPosition = qMakePair(_posR, _posR);
                    return false;
                } else {
                    currentContentPosition = qMakePair(_posL + l.length(), _posR);
                    return true;
                }

            }
        }

        pageHeader paginaServizio::currentHeader() {
            return *iter_headers;
        }

        bool paginaServizio::nextVariable() {
            //std::cout<<pagine.size();
            QList<pageVariable>::const_iterator i_end = variabili.end();

            if (conter_iter_variables == 0)
                iter_variables = variabili.begin();
            else
                ++iter_variables;

            ++conter_iter_variables;
            if (iter_variables == i_end) {
                conter_iter_variables = 0;
                return false;
            } else
                return true;
        }

        pageVariable paginaServizio::currentVariable() {
            return *iter_variables;
        }

        bool paginaServizio::nextCommand() {
            //std::cout<<pagine.size();
            QList<pageCommand>::const_iterator i_end = comandi.end();

            if (counter_iter_commands == 0) {
                iter_commands = comandi.begin();
            } else {
                ++iter_commands;
            }

            ++counter_iter_commands;
            if (iter_commands == i_end) {
                counter_iter_commands = 0;
                return false;
            } else
                return true;
        }

        pageCommand paginaServizio::currentCommand() {
            return *iter_commands;
        }

        bool paginaServizio::hasRawCommands() const {
            return !comandi.empty();
        }
    }
}
