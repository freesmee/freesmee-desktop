# -------------------
# Freesmee Project QT
# -------------------

#    Copyright (C) <2011>
#
#    <enrico bacis> <enrico.bacis@gmail.com>
#    <ivan vaccari> <grisson@jacksms.it>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This License extends all the files in the project (exclude the ones
#    with theri own license), even to the .ui form files.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

QT += network

TARGET = freesmee
TEMPLATE = app
INCLUDEPATH += "./include"

# Togliere il commento dalla riga seguente per creare una versione portable di Freesmee
# DEFINES += PORTABLE

macx { 
    SOURCES += MacOSspecific.cpp
    LIBS += /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
    LIBS += -L$$PWD/lib/osx
    ICON =
}

win32 { 
    LIBS += -L$$PWD/lib/win32
    RC_FILE = freesmee.rc
}

LIBS += -lticpp
DEFINES += TIXML_USE_TICPP

SOURCES += main.cpp \
    mainjacksms.cpp \
    captchadialog.cpp \
    servicesdialog.cpp \
    aggiungicontatto.cpp \
    opzionidialog.cpp \
    editcontattodialog.cpp \
    avvisoaccentidialog.cpp \
    libjacksms/xmlParserServerApiTicpp.cpp \
    libjacksms/xmlParserLocalApiTicpp.cpp \
    libjacksms/VariabileServizio.cpp \
    libjacksms/Utilities.cpp \
    libjacksms/StringTriplet.cpp \
    libjacksms/SmsSender.cpp \
    libjacksms/ShortMessage.cpp \
    libjacksms/Service.cpp \
    libjacksms/ServerApi.cpp \
    libjacksms/PhoneNumber.cpp \
    libjacksms/PaginaServizio.cpp \
    libjacksms/PageVariable.cpp \
    libjacksms/PageHeader.cpp \
    libjacksms/PageError.cpp \
    libjacksms/PageContent.cpp \
    libjacksms/PageAccept.cpp \
    libjacksms/LogSmsMessage.cpp \
    libjacksms/LogImMessage.cpp \
    libjacksms/Logger.cpp \
    libjacksms/LocalApi.cpp \
    libjacksms/FilesDirectory.cpp \
    libjacksms/Exceptions.cpp \
    libjacksms/EncodingUtf8.cpp \
    libjacksms/EncodingUrl.cpp \
    libjacksms/EncodingTransformAccents.cpp \
    libjacksms/EncodingRemoveAccents.cpp \
    libjacksms/EncodingNull.cpp \
    libjacksms/DateTime.cpp \
    libjacksms/Content.cpp \
    libjacksms/Contact.cpp \
    libjacksms/ConfiguredAccount.cpp \
    smswidget.cpp \
    contactwidget.cpp \
    contactwidgetfastbook.cpp \
    accountwidget.cpp \
    qcontact.cpp \
    qmymessage.cpp \
    libjacksms/DataTypesApi.cpp \
    statsdialog.cpp \
    plugindialog.cpp \
    libjacksms/PageCommand.cpp \
    libjacksms/Encrypter.cpp \
    libjacksms/LanguageManager.cpp \
    qlabelresult.cpp \
    libjacksms/netclientqhttp.cpp \
    libjacksms/SocketClient.cpp \
    messageloader.cpp \
    plugins/JackPluginHostInterfacer.cpp \
    libjacksms/EncodingIso88591.cpp \
    Configuration.cpp \
    libjacksms/libraryconfig.cpp \
    editaccountdialog.cpp \
    cambiaaccount.cpp \
    qtsingleapplication/qtsingleapplication.cpp \
    qtsingleapplication/qtlocalpeer.cpp \
    qrecipientwidget.cpp \
    faderwidget.cpp \
    multiplecheckdialog.cpp \
    listnames.cpp \
    namewidget.cpp \
    smstextedit.cpp \
    smslist.cpp \
    destinatariline.cpp \
    importgmaildialog.cpp \
    recipientcompleter.cpp \
    aboutdialog.cpp \
    libjacksms/streamer.cpp \
    libjacksms/json/json.cpp

HEADERS += mainjacksms.h \
    Types.h \
    captchadialog.h \
    servicesdialog.h \
    Configuration.h \
    aggiungicontatto.h \
    opzionidialog.h \
    JackUtils.h \
    editcontattodialog.h \
    plugins/JackPluginInterfacer.h \
    plugins/captcha/JackCaptchaPluginInterfacer.h \
    avvisoaccentidialog.h \
    pluginListItem.h \
    libjacksms/xmlParserServerApiTicpp.h \
    libjacksms/xmlParserServerApi.h \
    libjacksms/xmlParserLocalApiTicpp.h \
    libjacksms/xmlParserLocalApi.h \
    libjacksms/VariabileServizio.h \
    libjacksms/Utilities.h \
    libjacksms/tinyxml.h \
    libjacksms/tinystr.h \
    libjacksms/ticpprc.h \
    libjacksms/ticpp.h \
    libjacksms/StringTriplet.h \
    libjacksms/SmsSender.h \
    libjacksms/ShortMessage.h \
    libjacksms/Service.h \
    libjacksms/ServerApi.h \
    libjacksms/PhoneNumber.h \
    libjacksms/PaginaServizio.h \
    libjacksms/PageVariable.h \
    libjacksms/PageHeader.h \
    libjacksms/PageError.h \
    libjacksms/PageContent.h \
    libjacksms/PageAccept.h \
    libjacksms/NetClient.h \
    libjacksms/LogSmsMessage.h \
    libjacksms/LogImMessage.h \
    libjacksms/Logger.h \
    libjacksms/LocalApi.h \
    libjacksms/libJackSMS.h \
    libjacksms/FilesDirectory.h \
    libjacksms/Exceptions.h \
    libjacksms/EncodingUtf8.h \
    libjacksms/EncodingUrl.h \
    libjacksms/EncodingTransformAccents.h \
    libjacksms/EncodingRemoveAccents.h \
    libjacksms/EncodingNull.h \
    libjacksms/Encoding.h \
    libjacksms/DateTime.h \
    libjacksms/DataTypesApi.h \
    libjacksms/DataTypes.h \
    libjacksms/Content.h \
    libjacksms/Contact.h \
    libjacksms/ConfiguredAccount.h \
    smswidget.h \
    contactwidget.h \
    contactwidgetfastbook.h \
    accountwidget.h \
    qcontact.h \
    qmymessage.h \
    statsdialog.h \
    plugindialog.h \
    libjacksms/PageCommand.h \
    libjacksms/Encrypter.h \
    libjacksms/LanguageManager.h \
    plugins/scriba/JackScribaPluginInterfacer.h \
    qlabelresult.h \
    libjacksms/netclientqhttp.h \
    libjacksms/SocketClient.h \
    messageloader.h \
    plugins/JackPluginHostInterfacer.h \
    plugins/puppeteer/JackPuppeteerPluginInterfacer.h \
    libjacksms/EncodingIso88591.h \
    libjacksms/libraryconfig.h \
    editaccountdialog.h \
    cambiaaccount.h \
    qtsingleapplication/qtsingleapplication.h \
    qtsingleapplication/qtlocalpeer.h \
    qrecipientwidget.h \
    faderwidget.h \
    multiplecheckdialog.h \
    listnames.h \
    namewidget.h \
    smstextedit.h \
    smslist.h \
    destinatariline.h \
    importgmaildialog.h \
    recipientcompleter.h \
    aboutdialog.h \
    libjacksms/streamer.h \
    libjacksms/json/json.h

FORMS += mainjacksms.ui \
    captchadialog.ui \
    servicesdialog.ui \
    aggiungicontatto.ui \
    opzionidialog.ui \
    editcontattodialog.ui \
    avvisoaccentidialog.ui \
    statsdialog.ui \
    plugindialog.ui \
    editaccountdialog.ui \
    cambiaaccount.ui \
    multiplecheckdialog.ui \
    importgmaildialog.ui \
    aboutdialog.ui

RESOURCES += resources.qrc

OTHER_FILES += \
    freesmee.icns \
    freesmee.rc
