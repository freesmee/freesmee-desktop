#include "contactwidgetfastbook.h"
#include "libjacksms/libJackSMS.h"

#include <QLabel>


/*contactWidgetFastBook::contactWidgetFastBook(QString _id,QString _nome,QString _numero,QPixmap _icoServ,QString _accountId):id(_id),nomeContatto(_nome),accountId(_accountId),numero(_numero)
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
       labelIconInfo=new QLabel;
       setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
       adjustSize();

}*/
contactWidgetFastBook::contactWidgetFastBook(const libJackSMS::dataTypes::contact &_c,QPixmap _icoServ):id(_c.getId()),nomeContatto(_c.getName()),accountId(_c.getAccount()),numero(_c.getPhone().toString()),c(_c)
{
       hLayout = new QHBoxLayout;
       hLayout->setMargin(2);
       labelIconInfo=new QLabel;
       nome=new QLabel(_c.getName());
       icon=new QLabel;
       icon->setMaximumSize(16,16);
       icon->setPixmap(_icoServ);
       hLayout->addWidget(icon);
       hLayout->addWidget(nome);
       setLayout(hLayout);
       setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
       adjustSize();

}
libJackSMS::dataTypes::contact contactWidgetFastBook::getContact()const{
    return c;
}
QPixmap contactWidgetFastBook::getIcon()const{
    return  QPixmap(*icon->pixmap());

}
void contactWidgetFastBook::setInfoIcon(QPixmap _infoicon){
    labelIconInfo->deleteLater();
    labelIconInfo=new QLabel;
    labelIconInfo->setMaximumSize(16,16);
    labelIconInfo->setPixmap(_infoicon);
    hLayout->addWidget(labelIconInfo);
}
QString contactWidgetFastBook:: getName()const{
    return this->nomeContatto;
}
QString contactWidgetFastBook:: getContactId()const{
    return this->id;
}
contactWidgetFastBook * contactWidgetFastBook::clone() const{
    QPixmap p(*icon->pixmap());
    return new contactWidgetFastBook(c,p);

}
