#ifndef CONTACTWIDGET_H
#define CONTACTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ContactWidget : public QWidget
{
private:

    QLabel *nome,*numero,*iconServizio,*icon,*gruppo,*nomeAccount;
    QHBoxLayout *hLayout;
    QString id;
    QString nomeContatto;

public:
    ContactWidget(QString _id,QString _nome,QString _numero,QPixmap _icoServ,QString _gruppo,QString _accountName);
    QString getName()const;
    QString getContactId()const;
};

#endif // CONTACTWIDGET_H
