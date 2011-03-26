#include "DataTypesApi.h"
#include <QString>
#include <QRegExp>

namespace libJackSMS {

    namespace dataTypesApi {

        clientVersion::clientVersion(QString version) :
                major(0),
                minor(0),
                release('a'),
                build(0)
        {
            if (!version.isEmpty()) {
                QRegExp regExp;
                bool ok;

                regExp.setPattern(QString("^([0-9]{1,})\\.([0-9]{1,})\\-([abg]{1})([0-9]{1,})$"));
                if (regExp.exactMatch(version)) {

                    major = regExp.cap(1).toInt(&ok,10);
                    minor = regExp.cap(2).toInt(&ok,10);
                    release = regExp.cap(3).toStdString()[0];
                    build = regExp.cap(4).toInt(&ok,10);

                } else {

                    regExp.setPattern(QString("^([0-9]{1,})\\.([0-9]{1,})$"));
                    if (regExp.exactMatch(version)) {
                        major = regExp.cap(1).toInt(&ok,10);
                        minor = regExp.cap(2).toInt(&ok,10);
                    }

                }
            }
        }

        bool clientVersion::operator< (const clientVersion &client)
        {

            if (major < client.major)
                return true;
            else if (major > client.major)
                return false;

            if (minor < client.minor)
                return true;
            else if (minor > client.minor)
                return false;

            if (build < client.build)
                return true;
            else if (build > client.build)
                return false;

            if (release == 'a' && client.release == 'b')
                return true;
            else if (release == 'b' && client.release == 'g')
                return true;
            else
                return false;
        }

    }

}
