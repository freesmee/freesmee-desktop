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

#ifndef NETCLIENT_HH_GUARD
#define NETCLIENT_HH_GUARD 1

#include <QString>
#include <QUrl>

namespace libJackSMS
{
    namespace netClient
    {
        class abortedException : public std::exception
        {
        };

        class netClientGeneric
        {
            public:
                // Costruttore default. Insizializza il wrapper come un client socket se il parametro e' false
                netClientGeneric(){}
                virtual ~netClientGeneric(){}
                virtual bool setTimeout(int timeout)=0;
                virtual void IncludeHeaders()=0;
                virtual bool setCookieFile(const QString &_filename)=0;
                virtual bool setUseCookie(bool _useCookie)=0;
                virtual bool setUrl(const QString & _url)=0;
                virtual void setUserAgent(const QString &_useragent)=0;
                virtual QString readPage(bool _ret=false)=0;
                virtual bool insertFormData(const QString &_field,const QString &_value)=0;
                virtual QString submitPost(const QString & _url,bool _ret=false)=0;
                virtual QString submitGet(const QString & _url,bool _ret=false)=0;
                virtual QString currentUrl()=0;
                virtual bool findCaptcha(const QString &_left,const QString &_right)=0;
                virtual bool getFile(const QString &_url,QByteArray &_fileBytes)=0;
                virtual bool addHeader(const QString &_name,const QString &_value)=0;
                virtual bool setProxyServer(const QString & _proxyServer,const QString &_port,const QString &_proxyType)=0;
                virtual bool setProxyAuthentication(const QString &_proxyUsername, const QString &_proxyPassword)=0;
                virtual bool interrupt()=0;
                virtual bool getAborted()=0;
                virtual void clearCookies()=0;
                virtual bool hasError()=0;
        };
    }
}

#endif //NETCLIENT_HH_GUARD
