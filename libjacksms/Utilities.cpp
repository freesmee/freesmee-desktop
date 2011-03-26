#include "Utilities.h"
#include <QDir>
#include <QFile>

namespace libJackSMS {

    namespace utilities {

        /*void explode(std::string stringToExplode,std::list<std::string>& lista,char _separatore){
            if (stringToExplode.find(std::string(1,_separatore)+std::string(1,_separatore))!=std::string::npos)
                throw libJackSMS::exceptionMalformedString();
            if (stringToExplode[0]==_separatore || stringToExplode[stringToExplode.length()-1]==_separatore)
                throw libJackSMS::exceptionMalformedString();
            size_t pos=0,pos2;
            do{
                pos2=stringToExplode.find(std::string(1,_separatore),pos);
                if (pos2!=std::string::npos){
                    lista.push_back(stringToExplode.substr(pos,pos2-pos));
                    pos=pos2+1;
                }else{
                    lista.push_back(stringToExplode.substr(pos));
                }
            }while(pos2!=std::string::npos);
        }


        int toInt(std::string src){
            std::stringstream ss(std::stringstream::in | std::stringstream::out);
            ss<< src;
            int tmp;
            ss>>tmp;
            return tmp;
        }

*/

        QString Base64Encode(const QString &toEncode) {
            QByteArray data = toEncode.toAscii();
            return QString(data.toBase64());
        }

        QString Base64Decode(const QString &toDecode) {
            return QString(QByteArray::fromBase64(toDecode.toAscii()));
        }

        QByteArray Base64DecodeByte(const QString &toDecode) {
            return QByteArray::fromBase64(toDecode.toAscii());
        }

        bool deleteFile(const QString _fileName) {
            QString fileName = _fileName;
            fileName = QDir::toNativeSeparators(fileName);
            if (fileName.size() < 1) {
                return true;
            }

            QFile file(fileName);
            return file.remove();
        }

        bool deleteNotEmptyDir(const QString _dirName, bool removeDirectory) {
            QString dirName = dirName;
            QDir directory(QDir::toNativeSeparators(dirName));

            if (!directory.exists()) {
                return true;
            }

            QStringList dirs = directory.entryList(QDir::Dirs | QDir::Hidden);
            QStringList files = directory.entryList(QDir::Files | QDir::Hidden);

            QList<QString>::iterator f = files.begin();
            QList<QString>::iterator d = dirs.begin();

            bool error = false;

            for (; f != files.end(); ++f) {
                if (!deleteFile(directory.path() + '/' + (*f)))
                    error = true;
            }

            for (; d != dirs.end(); ++d) {
                if ((*d) == "." || (*d) == "..")
                    continue;

                if (!deleteNotEmptyDir(directory.path() + '/' + (*d)))
                    error = true;
            }

            if (removeDirectory)
                if(!directory.rmdir(directory.path()))
                    error = true;

            return !error;

        }

        bool deleteFilesInDirectory(const QString dirName) {
            return deleteNotEmptyDir(dirName, false);
        }

    }//end namespace utilities
}//end namespace libJackSMS
