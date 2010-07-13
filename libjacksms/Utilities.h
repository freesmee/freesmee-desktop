#include <QString>
#include <QByteArray>

#ifndef UTILITIES_HH_GUARD
#define UTILITIES_HH_GUARD 1



namespace libJackSMS{
    namespace utilities{
        /******************dichiarazioni************************/
        //void explode(std::string stringToExplode,std::list<std::string>& lista,char _separatore=',');

        //template <typename SRCTYPE>
        //std::string toString(SRCTYPE src);

        //int toInt(std::string src);

        QString Base64Encode(const QString &toEncode);
        QString Base64Decode(const QString &toDecode);
        QByteArray Base64DecodeByte(const QString &toDecode);

        /******************definizioni************************/
        /*template <typename SRCTYPE>
        std::string toString(SRCTYPE src){
            std::stringstream ss(std::stringstream::in | std::stringstream::out);
            ss<< src;
            std::string tmp;
            ss>>tmp;
            return tmp;
        }*/

    }
}

#endif //UTILITIES_HH_GUARD
