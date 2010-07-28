#include "Encrypter.h"
#include <cmath>
#include <QChar>
#include "Utilities.h"
namespace libJackSMS{
    namespace Encrypter{



        bool my_byte::my_xor(bool a, bool b){
                if ((a&&b) || ((!a)&&(!b)))
                    return false;
                else
                    return true;
        }

        my_byte::my_byte(){
            for(int i=0;i<8;i++)
                byte[i]=0;

        }
        my_byte::my_byte(char carattere){
            for(int i=0;i<8;i++)
                byte[i]=0;
            int i=7;
            while (carattere!=0){
                byte[i--]=carattere%2;
                carattere=carattere/2;
            }
        }
        my_byte::my_byte(const my_byte & _byte){
            for(int i=0;i<8;i++)
                byte[i]=_byte.byte[i];
        }
        my_byte my_byte::do_xor(const my_byte & _byte){
            my_byte b;
            for(int i=0;i<8;i++)
                b.byte[i]=my_xor(_byte.byte[i],byte[i]);
            return b;
        }
        char my_byte::getByte(){
            char final=0;
            for(int i=7;i>=0;i--)
                final=final+byte[i]*(std::pow(2,(7-i)));
            return final;
        }


        Encrypter::Encrypter()
        {
        }
        QString  Encrypter::base(QString _str,QString _pass){

            QString fi;
            for(size_t i=0,x=0;i<_str.length();++i,++x){
                if (x==_pass.length())
                    x=0;
                my_byte a(_str.at(i).toAscii());
                my_byte b(_pass.at(x).toAscii());
                my_byte r(a.do_xor(b));
                fi=fi.append(QChar(r.getByte()));

            }

            return fi;
        }
        QString Encrypter::encrypt(QString _strToEnc){
            if (_strToEnc.isEmpty())
                return _strToEnc;
            _strToEnc=_strToEnc.append("control");


            QString res=utilities::Base64Encode(base(utilities::Base64Encode(_strToEnc),"internalpassword"));
            return res;

        }
        QString Encrypter::decrypt(const QString &_strToDec){

            if (_strToDec.isEmpty())
                return _strToDec;
            QString r=utilities::Base64Decode(base(utilities::Base64Decode(_strToDec),"internalpassword"));
            if (r.length()<7)
                return "";
            QString control=r.mid(r.length()-7,7);
            if (control=="control")
                return r.left(r.length()-7);
            else
                return "";

        }

    }
}
