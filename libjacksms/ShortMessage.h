#include <QString>
#ifndef SHORTMESSAGE_HH
#define SHORTMESSAGE_HH 1

namespace libJackSMS{
    namespace dataTypes{
        class shortMessage{
            private:
                QString text;
            public:
                shortMessage();
                shortMessage(QString _text);
                void setText(const QString & _text);
                QString getText();



        };
    }
}
#endif //SHORTMESSAGE_HH

