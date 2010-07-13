#include "Utilities.h"
namespace libJackSMS{
    namespace utilities{

        /*void explode(std::string stringToExplode,std::list<std::string>& lista,char _separatore){
            if (stringToExplode.find(std::string(1,_separatore)+std::string(1,_separatore))!=std::string::npos)
                throw libJackSMS::exceptionMalformedString();
            if (stringToExplode[0]==_separatore || stringToExplode[stringToExplode.length()-1]==_separatore)
                throw libJackSMS::exceptionMalformedString();
            size_t pos=0,pos2;
            do{
                pos2=stringToExplode.find(std::string(1,_separatore),pos);
                if (pos2!=std::string::npos){
                    lista.push_back(stringToExplode.substr(pos,pos2-pos));
                    pos=pos2+1;
                }else{
                    lista.push_back(stringToExplode.substr(pos));
                }
            }while(pos2!=std::string::npos);
        }


        int toInt(std::string src){
            std::stringstream ss(std::stringstream::in | std::stringstream::out);
            ss<< src;
            int tmp;
            ss>>tmp;
            return tmp;
        }

*/

        QString Base64Encode(const QString &toEncode){
            QByteArray data=toEncode.toAscii();
            return QString(data.toBase64());
        }

        QString Base64Decode(const QString &toDecode){
            return QString(QByteArray::fromBase64(toDecode.toAscii()));
        }
        QByteArray Base64DecodeByte(const QString &toDecode){
            return QByteArray::fromBase64(toDecode.toAscii());
        }

    }//end namespace utilities
}//end namespace libJackSMS
