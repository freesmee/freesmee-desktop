
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

#include "ReportOperationSendSmsNothing.h"
#include <QString>
namespace libJackSMS{


    reportOperationSendSmsNothing::reportOperationSendSmsNothing(){
    }
    reportOperationSendSmsNothing::~reportOperationSendSmsNothing(){
    }
    void reportOperationSendSmsNothing::doOperation(){
    }
    void reportOperationSendSmsNothing::doOperation(const QString & _text){
    }
    QString reportOperationSendSmsNothing::doCaptchaOperation(QByteArray & data){
        /****
        netClient::MemoryStruct *captchaBytes=(netClient::MemoryStruct*) data;
        std::ofstream myfile;
        myfile.open ((directories::DataDirectory()+"captcha.png").c_str(), std::ios::out | std::ios::app | std::ios::binary);
        for(unsigned int i=0;i<captchaBytes->size;i++)
            myfile <<captchaBytes->memory[i];
        myfile.close();
        QString cap;
        std::cin>>cap;
        return cap;
        *****/
        return QString();
    }
    void reportOperationSendSmsNothing::doOperationError(const QString & _text){
    }
    void reportOperationSendSmsNothing::doOperationSuccess(const QString & _text){
    }

}


