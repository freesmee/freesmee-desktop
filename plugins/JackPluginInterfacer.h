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

#include <QtCore>
#include <QtGui>

#ifndef JACKPLUGININTERFACER_H
#define JACKPLUGININTERFACER_H

enum PluginStatus{Ready, Broken};
enum PluginType{CaptchaDecoder, Scriba};

struct PluginInfo{
    PluginType pluginType;
    QString name;
    QString author;
    QString description;
    QString version;

    QString pluginTypeAsString(){
        switch (pluginType){
        case CaptchaDecoder:
            return "CaptchaDecoder";
        default:
            return "Unknown/Test";
        }
    }
};



//questa è un'interfaccia da cui ereditare per implementare un plugin
//e da includere per caricare un plugin di JackSMS
class JackPluginInterface : public QObject

{
    Q_OBJECT
 public:
     virtual ~JackPluginInterface() {}

     virtual PluginStatus getPluginStatus() = 0;
     virtual PluginInfo getPluginInfo() = 0;
 public slots:
     virtual void configure() = 0;
 };


//dichiara che questa è un'interfaccia QT
Q_DECLARE_INTERFACE(JackPluginInterface, "jackSMS.Plugins/1.0")

#endif // JACKPLUGININTERFACER_H
