#ifndef ACCOUNTWIDGET_H
#define ACCOUNTWIDGET_H

#include <QWidget>
#include <QLabel>

#include <QHBoxLayout>
#include "libjacksms/libJackSMS.h"



class accountWidget : public QWidget
{
private:

    QLabel *nome,*icon;
    QHBoxLayout *hLayout;
    QString id;

public:
    accountWidget(QString _id,QString _nome,QPixmap _icoServ);

    QString getContactId()const;
    QString getAccountId()const;
};
#endif // ACCOUNTWIDGET_H
