#include <QString>
#include <libjacksms/libJackSMS.h>
#ifndef QCONTACT_H
#define QCONTACT_H

class QContact
{

private:
    QString nome;
    libJackSMS::dataTypes::phoneNumber telefono;
    QString gruppo;
    QString account;
    QString id;
public:
    QContact(const QString & _nome,const libJackSMS::dataTypes::phoneNumber & _telefono,const QString & _gruppo, const QString & _account);
    QString getName()const;
    QString getGroup()const;
    QString getAccount()const;
    libJackSMS::dataTypes::phoneNumber getPhone()const;
    void setId(const QString & _id);
    void setName(const QString & _name);
    QString getId()const;
};

#endif // QCONTACT_H
