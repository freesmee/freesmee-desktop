#include "multiplecheckdialog.h"
#include "ui_multiplecheckdialog.h"
#include "contactwidgetfastbook.h"
#include <QMultiMap>
multipleCheckDialog::multipleCheckDialog(const libJackSMS::dataTypes::phoneBookType & _rubrica,const libJackSMS::dataTypes::configuredServicesType & _elencoServiziConfigurati,const libJackSMS::dataTypes::servicesType & _elencoServizi,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::multipleCheckDialog),
    rubrica(_rubrica),
    elencoServiziConfigurati(_elencoServiziConfigurati),
    elencoServizi(_elencoServizi)
{
    ui->setupUi(this);
    writeToGui();
}

multipleCheckDialog::~multipleCheckDialog()
{
    delete ui;
}

void multipleCheckDialog::writeToGui(){
    libJackSMS::dataTypes::phoneBookType::const_iterator i=rubrica.begin();
    libJackSMS::dataTypes::phoneBookType::const_iterator i_end=rubrica.end();

    QMultiMap<QString,contactWidgetFastBook*> fastList;
    QString filter=ui->lineEdit->text();
    for(;i!=i_end;++i){
        if (i->getName().contains(filter,Qt::CaseInsensitive)){
            QIcon ico;
            libJackSMS::dataTypes::configuredServicesType::const_iterator x=elencoServiziConfigurati.find(i->getAccount());
            if (x==elencoServiziConfigurati.end()){
                ico=QIcon(":/resource/ico_contact.png");

            }else{
                QString serv=x.value().getService();
                libJackSMS::dataTypes::servicesType::const_iterator tmp=elencoServizi.find(serv);
                ico=tmp->getIcon();

            }
            contactWidgetFastBook *ww=new contactWidgetFastBook(i.value(),ico.pixmap(16,16),true);
            fastList.insert(i->getName().toUpper(),ww);

        }
    }

    if (fastList.size()>0){
        QMultiMap<QString,contactWidgetFastBook*>::ConstIterator xx=fastList.begin();
        QMultiMap<QString,contactWidgetFastBook*>::ConstIterator xx_end=fastList.end();
        for(;xx!=xx_end;++xx){
           QListWidgetItem *item = new QListWidgetItem;
           item->setSizeHint(xx.value()->size());
           ui->listWidget->addItem(item);
           ui->listWidget->setItemWidget(item, xx.value());
        }
    }
}

void multipleCheckDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void multipleCheckDialog::on_pushButton_clicked()
{
    this->close();
}

void multipleCheckDialog::on_lineEdit_textEdited(QString text)
{
    int c=ui->listWidget->count();
    for(int x=0;x<c;x++){
        QListWidgetItem *item=ui->listWidget->item(x);
        contactWidgetFastBook *w=static_cast<contactWidgetFastBook*>(ui->listWidget->itemWidget(item));
        if (w->getName().contains(text,Qt::CaseInsensitive))
            ui->listWidget->setItemHidden(item,false);
        else
            ui->listWidget->setItemHidden(item,true);
    }
}

void multipleCheckDialog::on_pushButton_2_clicked()
{
    QList<QRecipientWidget*> l;
    int c=ui->listWidget->count();
    for(int x=0;x<c;x++){
        QListWidgetItem *item=ui->listWidget->item(x);
        contactWidgetFastBook *w=static_cast<contactWidgetFastBook*>(ui->listWidget->itemWidget(item));
        if (w->isChecked()){
            QRecipientWidget *wi=new QRecipientWidget(w->getContact().getName(),w->getContact().getAccount(),w->getContact().getPhone(),w->getIcon());
            l.push_back(wi);
        }
    }
    emit this->addRecipients(l);
    this->close();

}
