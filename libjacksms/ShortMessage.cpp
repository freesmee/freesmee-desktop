#include <QString>
#include "ShortMessage.h"

namespace libJackSMS{
    namespace dataTypes{
        shortMessage::shortMessage(QString _text):text(_text){
        }
        shortMessage::shortMessage(){
        }
        void shortMessage::setText(const QString & _text){
            text=_text;
        }

        QString shortMessage::shortMessage::getText(){
            return text;
        }
    }
}
