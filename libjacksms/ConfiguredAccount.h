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

#include <QMap>
#include <QString>
#ifndef CONFIGUREDACCOUNT_HH
#define CONFIGUREDACCOUNT_HH 1

namespace libJackSMS{

    namespace dataTypes{

        class configuredAccount{
            private:
                QString nome;
                QString id;
                QString servizio;

                QMap<QString,QString> statistiche;
                int indiceStatistica;
                QMap<QString,QString>::const_iterator iterStatistiche;

                QMap<QString,QString> dati;
                int indiceDato;
                QMap<QString,QString>::const_iterator iterDati;

                QMap<QString,QString> opzioni;
                int indiceOpzione;
                QMap<QString,QString>::const_iterator iterOpzioni;

            public:
                configuredAccount();
                void setName(const QString &_value);
                void setId(const QString &_value);
                void setService(const QString &_value);
                void setStat(const QString &_statName,const QString &_statValue);
                void setData(const QString &_dataName,const QString &_dataValue);
                bool updateData(const QString &_dataName,const QString &_dataValue);
                void setOption(const QString &_optName,const QString &_optValue);
                QString getName()const;
                QString getId()const;
                QString getService()const;
                QString getStat(const QString &_statName)const;
                QString getData(const QString &_dataName)const;
                QString getOption(const QString &_optName)const;
                //void updateata(const QString &_dataName,const QString &_dataValue);

                bool increaseStatValue(QString _varname);
                bool nextData();
                /*ritorna il nome del dato corrente sotto all'iteratore*/
                QString currentData() const;

                bool nextOption();
                QString currentOption() const;

                bool nextStat();
                QString currentStat() const;
                void sumStats(configuredAccount other);
        };

    }

}
#endif //CONFIGUREDACCOUNT_HH
