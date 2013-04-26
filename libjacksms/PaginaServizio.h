/*
    Copyright (C) <2011>

    <enrico bacis> <enrico.bacis@gmail.com>
    <ivan vaccari> <grisson@jacksms.it>

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

#ifndef PAGINASERVIZIO_HH
#define PAGINASERVIZIO_HH 1

#include <QString>
#include <QList>
#include <QPair>
#include "PageAccept.h"
#include "PageError.h"
#include "PageHeader.h"
#include "PageContent.h"
#include "PageVariable.h"
#include "PageCommand.h"

namespace libJackSMS
{
    namespace dataTypes
    {
        class paginaServizio
        {
            private:
                QString url;
                QString metodo;
                bool captcha;
                QString pageHtml;
                QString condizione;
                QList<pageError> errori;
                QList<pageAccept> accettanti;
                QList<pageHeader> intestazioni;
                QList<pageHeader>::const_iterator iter_headers;
                int conter_iter_headers;
                QList<pageVariable> variabili;
                QList<pageVariable>::const_iterator iter_variables;
                int conter_iter_variables;
                QList<pageContent> contenuti;
                int counter_iter_contents;
                QList<pageContent>::const_iterator iter_contents;
                pageError currentError;
                pageAccept currentAccept;
                QPair<int, int> currentContentPosition;
                bool ha_condizione;
                //Nota: il parametro sleepbefore è in secondi (come passato dal server)
                int sleepbefore;

                QList<pageCommand> comandi;
                QList<pageCommand>::const_iterator iter_commands;
                int counter_iter_commands;

            public:
                paginaServizio();
                void setUrl(const QString &_url);
                QString getUrl() const;
                void setMethod(const QString &_method);
                QString getMethod() const;
                void setIsCaptcha(bool _iscaptcha);
                bool getIsCaptcha() const;
                void setSleepbefore(int _sleepbefore);
                int getSleepbefore() const;
                void setError(const pageError &_error);
                void setRetreivedHtml(const QString &_html);
                bool checkErrors();
                dataTypes::pageError getError();
                bool checkAccepts();
                dataTypes::pageAccept getAccept();
                void setCondition(const QString &_condizione);
                QString getCondition() const;
                bool hasCondition() const;
                void setVariable(const pageVariable &_var);
                void setContent(const pageContent &_con);
                bool nextContent();
                QString currentContentName() const;
                QString currentContentValue() const;
                bool currentContentFound();
                void setAccept(const pageAccept &_acc);
                void setHeader(const pageHeader &_header);
                bool nextHeader();
                pageHeader currentHeader();
                bool nextVariable();
                pageVariable currentVariable();

                void setCommand(const pageCommand &_cmd);
                bool nextCommand();
                pageCommand currentCommand();
                bool hasRawCommands() const;
                bool currentContentToEncode() const;

        };
    }
}

#endif //PAGINASERVIZIO_HH
