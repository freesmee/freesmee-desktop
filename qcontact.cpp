#include <QString>
#include "qcontact.h"


QContact::QContact(const QString & _nome,const libJackSMS::dataTypes::phoneNumber & _telefono,const QString & _gruppo, const QString & _account):nome(_nome),telefono(_telefono),gruppo(_gruppo),account(_account){}

QString QContact::getName() const{
    return nome;
}
QString QContact::getGroup()const{
    return gruppo;
}
QString QContact::getAccount()const{
    return account;
}
libJackSMS::dataTypes::phoneNumber QContact::getPhone()const{
    return telefono;
}
void QContact::setId(const QString & _id){
    id=_id;
}
QString QContact::getId()const{
    return id;
}
void QContact::setName(const QString & _name){
    nome=_name;
}
void QContact::setAccount(QString _account){
    account=_account;
}
