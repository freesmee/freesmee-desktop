#include "contactwidgetfastbook.h"
#include "libjacksms/libJackSMS.h"

#include <QLabel>


contactWidgetFastBook::contactWidgetFastBook(QString _id,QString _nome,QString _numero,QPixmap _icoServ,QString _accountId):id(_id),nomeContatto(_nome),accountId(_accountId)
{
       hLayout = new QHBoxLayout;
       hLayout->setMargin(2);

       nome=new QLabel(_nome);
       //icon->setMaximumSize(1000,16);
       //nome->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
       icon=new QLabel;
       icon->setMaximumSize(16,16);
       //icon->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
       icon->setPixmap(_icoServ);
       hLayout->addWidget(icon);
       hLayout->addWidget(nome);
       setLayout(hLayout);
       //setMaximumSize(1000,16);
       setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
       adjustSize();

}
QString contactWidgetFastBook:: getName()const{
    return this->nomeContatto;
}
QString contactWidgetFastBook:: getContactId()const{
    return this->id;
}
