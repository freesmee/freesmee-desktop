#include "Encrypter.h"
#include <cmath>
#include <QChar>
#include "Utilities.h"
#include <iostream>
namespace libJackSMS{
    namespace Encrypter{




        Encrypter::Encrypter()
        {
        }
        QString  Encrypter::base(QString _str,int multiplier){

            QString fi;
            for(int i=0,x=0;i<_str.length();++i,++x){
                fi.append(_str.at(i)+30*multiplier);
            }

            return fi;
        }
        QString Encrypter::encrypt(QString _strToEnc){
            if (_strToEnc.isEmpty())
                return _strToEnc;
            _strToEnc=_strToEnc.append("control");
            QString res=_strToEnc;
            res=utilities::Base64Encode(res);
            res=base(res,+1);
            res=utilities::Base64Encode(res);

            return res;

        }
        QString Encrypter::decrypt(const QString &_strToDec){

            if (_strToDec.isEmpty())
                return _strToDec;

            QString r=utilities::Base64Decode(_strToDec);
            r=base(r,-1);
            r=utilities::Base64Decode(r);

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
