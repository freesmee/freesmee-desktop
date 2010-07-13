#ifndef ENCRYPTER_H
#define ENCRYPTER_H
#include <QString>
namespace libJackSMS{
    namespace Encrypter{
        class my_byte{
            private:
                bool my_xor(bool a, bool b);
            protected:
                bool byte[8];
            public:
                my_byte();
                my_byte(char carattere);
                my_byte(const my_byte & _byte);
                my_byte do_xor(const my_byte & _byte);
                char getByte();
        };

        class Encrypter
        {
        private:
            static QString base(QString _str,QString _pass);
        public:
            Encrypter();

            static QString encrypt(QString _strToEnc);
            static QString decrypt(const QString &_strToDec);
        };
    }
}
#endif // ENCRYPTER_H
