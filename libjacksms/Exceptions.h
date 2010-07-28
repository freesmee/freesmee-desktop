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


#include <QString>

#ifndef EXCEPTION_HH_GUARD
#define EXCEPTION_HH_GUARD 1

namespace libJackSMS{
    class exceptionGeneric : public std::exception{
    };
    class exceptionMalformedString : public std::exception{
    };

    class exceptionPhoneNumber : public std::exception{
    };
    class exceptionSharedMemory : public std::exception{
    };
    class exceptionRegExpr : public std::exception{
    };

    class exceptionInterruptedSend : public std::exception{
    };
    class exceptionXmlNotFound : public std::exception{
    };
    class exceptionXmlError: public std::exception{
        private:
            QString details;
        public:
            exceptionXmlError(QString _error);
            ~exceptionXmlError() throw();
            const char* what() const throw();
    };
    class exceptionFieldNotFound: public std::exception{
        private:
            QString details;
        public:
            exceptionFieldNotFound(QString _error);
            ~exceptionFieldNotFound() throw();
            const char* what() const throw();
    };

    class exceptionSomethingWrong: public std::exception{
        private:
            QString details;
        public:
            exceptionSomethingWrong(QString _error);
            ~exceptionSomethingWrong() throw();
            const char* what() const throw();
    };


}

#endif //LEXCEPTION_HH_GUARD
