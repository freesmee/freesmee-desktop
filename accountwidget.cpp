#include "accountwidget.h"
#include "libjacksms/libJackSMS.h"

#include <QLabel>


accountWidget::accountWidget(QString _id,QString _nome,QPixmap _icoServ):id(_id)
{
       hLayout = new QHBoxLayout;
       //hLayout->setMargin(2);

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
QString accountWidget::getAccountId()const{
    return this->id;
}
