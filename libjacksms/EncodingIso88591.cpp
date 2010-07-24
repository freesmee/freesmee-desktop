#include "EncodingIso88591.h"
namespace libJackSMS{
    encodingISO88591::encodingISO88591()
    {
    }
    QString encodingISO88591::getEncodedString(QString _text){
        QByteArray _t=_text.toLatin1();
        return QString(_t);
    }
    QString encodingISO88591::getEncodedAndUrlString(QString _text){
        QByteArray _t=_text.toLatin1().toPercentEncoding();
        return QString(_t);

    }

}
