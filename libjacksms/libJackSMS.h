/** \mainpage libJackSMS

 *
 * libJackSMS e' una collezione di classi che implementano le funzionalita' necessarie per poter inviare sms tramite il servizio offerto da jacksms.it<br>
 * E' completamente scritta in C++/Qt ed utilizza estensivamente le peculiarita' delle libreie Qt.<br>
 * Dipende solamente dalla libreria XML ticpp ( http://code.google.com/p/ticpp/ ) da cui pero' e' in progetto il porting a Qt.<br>


 * <b>Informazioni:</b>
 * \li <a href="http://www.jacksms.it">jacksms.it</a>
 * \li <a href="http://code.google.com/p/libjacksms/">libJackSMS home page</a><p>

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

#include "netclientqhttp.h"
#include "NetClient.h"
#include "PageAccept.h"
#include "PageContent.h"
#include "PageError.h"
#include "PageHeader.h"
#include "PageVariable.h"
#include "PaginaServizio.h"
#include "PhoneNumber.h"
#include "ReportOperationSendSms.h"
#include "ReportOperationSendSmsNothing.h"
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

