#include <string>
#include <sstream>
#include <QString>

#ifndef JACKUTILS_H
#define JACKUTILS_H





namespace jackUtils{

/*QString Base64Encode(const QString &toEncode);

QString Base64Decode(const QString &toDecode);
*/

template <typename SRCTYPE>
QString ToQString(SRCTYPE src){
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss<< src;
    std::string tmp;
    ss>>tmp;
    return QString(tmp.c_str());
}
}

#endif // JACKUTILS_H
