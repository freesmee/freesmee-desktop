#ifndef ENCRYPTER_H
#define ENCRYPTER_H
#include <QString>
namespace libJackSMS{
    namespace Encrypter{
        class Encrypter
        {
        private:
            static QString base(QString _str,int multiplier);
        public:
            Encrypter();

            static QString encrypt(QString _strToEnc);
            static QString decrypt(const QString &_strToDec);
        };
    }
}
#endif // ENCRYPTER_H
