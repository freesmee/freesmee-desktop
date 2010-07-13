#include <QList>
#include "Logger.h"
#include "FilesDirectory.h"
#include <QFile>
#include <QString>

namespace libJackSMS{



    logger::logger(){
    }
    void logger::addNotice(const QString &_notice){
        noticeList.push_back(_notice);
        save(directories::DataDirectory()+QString("live_generated_log.txt"));
    }
    bool logger::save(){
        return save(directories::DataDirectory()+QString("debug.txt"));
    }

    bool logger::save(const QString &_filename){
        QFile fi(_filename);

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
