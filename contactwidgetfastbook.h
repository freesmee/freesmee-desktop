#ifndef CONTACTWIDGETFASTBOOK_H
#define CONTACTWIDGETFASTBOOK_H


#include <QWidget>
#include <QLabel>

#include <QHBoxLayout>
#include "libjacksms/libJackSMS.h"


class contactWidgetFastBook : public QWidget
{
private:

    QLabel *nome,*icon;
    QHBoxLayout *hLayout;
    QString id;
    QString nomeContatto;
    QString accountId;
    QString numero;
    libJackSMS::dataTypes::contact c;
    QLabel *labelIconInfo;
public:
    //contactWidgetFastBook(QString _id,QString _nome,QString _numero,QPixmap _icoServ,QString _accountId);
    contactWidgetFastBook(const libJackSMS::dataTypes::contact &_c,QPixmap _icoServ);
    QString getName()const;
    QString getContactId()const;
    QString getAccountId()const;
    contactWidgetFastBook *clone() const;
    libJackSMS::dataTypes::contact getContact()const;
    QPixmap getIcon()const;
    void setIcon(QPixmap _pix);
    void showIcon(bool value);
    void setInfoIcon(QPixmap _infoicon, const QString &_tooltip);
};


#endif // CONTACTWIDGETFASTBOOK_H
