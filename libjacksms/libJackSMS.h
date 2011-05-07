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

/** \mainpage libJackSMS

 *
 * libJackSMS e' una collezione di classi che implementano le funzionalita' necessarie per poter inviare sms tramite il servizio offerto da jacksms.it<br>
 * E' completamente scritta in C++/Qt ed utilizza estensivamente le peculiarita' delle libreie Qt.<br>
 * Dipende solamente dalla libreria XML ticpp ( http://code.google.com/p/ticpp/ ) da cui pero' e' in progetto il porting a Qt.<br>


 * <b>Informazioni:</b>
 * \li <a href="http://www.freesmee.com">freesmee.com</a><p>

 */


#ifndef LIBJACKSMS_H
#define LIBJACKSMS_H

#include "ConfiguredAccount.h"
#include "Contact.h"
#include "Content.h"
#include "DataTypesApi.h"
#include "DataTypes.h"
#include "DateTime.h"
#include "Encoding.h"
#include "EncodingNull.h"
#include "EncodingRemoveAccents.h"
#include "EncodingTransformAccents.h"
#include "EncodingUrl.h"
#include "EncodingUtf8.h"
#include "Exceptions.h"
#include "FilesDirectory.h"
#include "libJackSMS.h"
#include "LocalApi.h"
#include "Logger.h"
#include "LogImMessage.h"
#include "LogSmsMessage.h"
#include "Encrypter.h"

#include "netclientqhttp.h"
#include "NetClient.h"
#include "PageAccept.h"
#include "PageContent.h"
#include "PageError.h"
#include "PageHeader.h"
#include "PageVariable.h"
#include "PaginaServizio.h"
#include "PhoneNumber.h"
#include "ServerApi.h"
#include "Service.h"
#include "ShortMessage.h"
#include "SmsSender.h"
#include "StringTriplet.h"
#include "ticpp.h"
#include "ticpprc.h"
#include "tinystr.h"
#include "tinyxml.h"
#include "Utilities.h"
#include "VariabileServizio.h"
#include "xmlParserLocalApi.h"
#include "xmlParserLocalApiTicpp.h"
#include "xmlParserServerApi.h"
#include "xmlParserServerApiTicpp.h"
#include "ProxyConfig.h"
#include "LanguageManager.h"
#endif //LIBJACKSMS_H

