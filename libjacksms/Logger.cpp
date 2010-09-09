#include <QList>
#include "Logger.h"
#include "FilesDirectory.h"
#include <QFile>
#include <QString>

namespace libJackSMS{



    logger::logger(QString _filename):filename(_filename){
    }
    void logger::addNotice(const QString &_notice){
        noticeList.push_back(_notice);
        QFile fi(filename);
        if (fi.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)){

                fi.write(_notice.toAscii());
                fi.write("\n");

            fi.close();

        }


    }

    void logger::cancelLog(){

        QFile fi(filename);
        if (fi.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
            fi.close();
    }
    bool logger::save(){

            QFile fi(filename);

            if (fi.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
                QList<QString>::const_iterator i=noticeList.begin();
                QList<QString>::const_iterator i_end=noticeList.end();
                for (;i!=i_end;++i){
                    fi.write((*i).toAscii());
                    fi.write("\n");
                }
                fi.close();
                return true;
            }else{
                return false;
            }

    }



}
