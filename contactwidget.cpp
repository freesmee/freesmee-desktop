#include "contactwidget.h"
#include <QLabel>
ContactWidget::ContactWidget(QString _id,QString _nome,QString _numero,QPixmap _icoServ,QString _gruppo,QString _accountName):id(_id),nomeContatto(_nome)
{
       hLayout = new QHBoxLayout;
       //vLayout = new QVBoxLayout;
       nome=new QLabel(_nome);
       //nome->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
       numero=new QLabel(_numero);
       icon=new QLabel;
       icon->setMaximumSize(16,200);
       icon->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
       icon->setPixmap(QPixmap(":/resource/ico_contact.png"));
       nomeAccount=new QLabel(_accountName);
       //numero->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
       iconServizio=new QLabel;
       iconServizio->setPixmap(_icoServ);
       iconServizio->setMaximumSize(16,200);
       iconServizio->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Minimum);
       gruppo=new QLabel(_gruppo);

       //gruppo->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
       hLayout->addWidget(icon);
       hLayout->addWidget(nome);
       hLayout->addWidget(numero);
       hLayout->addWidget(iconServizio);
       hLayout->addWidget(nomeAccount);

       hLayout->addWidget(gruppo);
       //vLayout->addLayout(hLayout);
       setLayout(hLayout);
       setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
       adjustSize();

}
QString ContactWidget:: getName()const{
    return this->nomeContatto;
}
QString ContactWidget:: getContactId()const{
    return this->id;
}

bool ContactWidget:: searchMatch(QString _txt)const{
    return (!(-1==nome->text().indexOf(_txt,0,Qt::CaseInsensitive))) || (!(-1==numero->text().indexOf(_txt,0,Qt::CaseInsensitive)))|| (!(-1==nomeAccount->text().indexOf(_txt,0,Qt::CaseInsensitive))) ;
}
