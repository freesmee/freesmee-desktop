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
public:
    contactWidgetFastBook(QString _id,QString _nome,QString _numero,QPixmap _icoServ,QString _accountId);
    QString getName()const;
    QString getContactId()const;
    QString getAccountId()const;
};


#endif // CONTACTWIDGETFASTBOOK_H
