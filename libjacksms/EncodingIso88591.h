#ifndef ENCODINGISO88591_H
#define ENCODINGISO88591_H
#include "Encoding.h"
namespace libJackSMS{
    class encodingISO88591 : public encodingGeneric
    {
    public:
        encodingISO88591();
        QString getEncodedString(QString _text);
        QString getEncodedAndUrlString(QString _text);
    };
}
#endif // ENCODINGISO88591_H
