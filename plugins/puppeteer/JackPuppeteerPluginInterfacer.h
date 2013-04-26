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

#ifndef JACKSCRIBAPLUGININTERFACER_H
#define JACKSCRIBAPLUGININTERFACER_H

#include <QtCore>
#include <QtGui>

#include "../JackPluginInterfacer.h"
#include "../JackPluginHostInterfacer.h"

//questa è un'interfaccia da cui ereditare per implementare un plugin puppetter (burattinaio)
//questo tipo di plugins ha un riferimento all'istanza di JackPluginHostInterface e puo' pertanto ascoltare i messaggi in arrivo ed eseguire varie operazioni.
class JackPuppeteerPluginInterface : public JackPluginInterface
 {
 private:
    bool initialized = false;
 public:
     virtual ~JackPuppeteerPluginInterface() {}
     virtual initialize(JackPluginHostInterface*);
 };


//dichiara che questa è un'interfaccia QT
Q_DECLARE_INTERFACE(JackPuppeteerPluginInterface, "jackSMS.PuppeteerPlugins.VodafoneSMS/1.0")

#endif // JACKSCRIBAPLUGININTERFACER_H
