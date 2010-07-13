#include "DataTypesApi.h"
#include <QString>
#include <QRegExp>
namespace libJackSMS{

    namespace dataTypesApi{

        clientVersion::clientVersion(QString version):mayor(0),minor(0),release('a'),build(0){
            if (!version.isEmpty()){

                QRegExp regExp;
                regExp.setPattern(QString("^([0-9]{1,})\\.([0-9]{1,})\\-([abg]{1})([0-9]{1,})$"));
                if (regExp.exactMatch(version)){
                    bool ok;
                    mayor=regExp.cap(1).toInt(&ok,10);
                    minor=regExp.cap(2).toInt(&ok,10);
                    release=regExp.cap(3).toStdString()[0];
                    build=regExp.cap(4).toInt(&ok,10);
                }

            }

        }
        bool  clientVersion::operator< ( const clientVersion & client){
            if (mayor <client.mayor) return true;
            if (minor <client.minor) return true;
            if (build <client.build) return true;
            if (release=='a' && client.release=='b') return true;
            if (release=='b' && client.release=='g') return true;
            return false;


        }



    }

}
