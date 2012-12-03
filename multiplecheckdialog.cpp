/*
    Copyright (C) <2011>

    <enrico bacis> <enrico.bacis@gmail.com>
    <ivan vaccari> <grisson@jacksms.it>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    You can't modify the adv system, to cheat it.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "multiplecheckdialog.h"
#include "ui_multiplecheckdialog.h"
#include "contactwidgetfastbook.h"
#include <QMultiMap>

multipleCheckDialog::multipleCheckDialog(const libJackSMS::dataTypes::phoneBookType &_rubrica, const libJackSMS::dataTypes::configuredServicesType &_elencoServiziConfigurati, const libJackSMS::dataTypes::servicesType &_elencoServizi, MainJackSMS *_padre, QWidget *parent) :
    QDialog(parent),
    padre(_padre),
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

void multipleCheckDialog::writeToGui()
{

    QMultiMap<QString,contactWidgetFastBook*> fastList;
    QString filter = ui->lineEdit->text();

    for (libJackSMS::dataTypes::phoneBookType::const_iterator i = rubrica.begin(); i != rubrica.end(); ++i) {

        //Controllo che non sia già presente nella lista destinatari
        if (!padre->isInRecipients(i.value().getPhone()))
        {
            if (i->getName().contains(filter, Qt::CaseInsensitive))
            {
                QIcon ico;
                libJackSMS::dataTypes::configuredServicesType::const_iterator x = elencoServiziConfigurati.find(i->getAccount());

                if (x == elencoServiziConfigurati.end())
                {
                    ico = QIcon(":/resource/ico_contact.png");
                }
                else
                {
                    QString serv = x.value().getService();
                    libJackSMS::dataTypes::servicesType::const_iterator tmp = elencoServizi.find(serv);
                    ico = tmp->getIcon();
                }

                contactWidgetFastBook *ww = new contactWidgetFastBook(i.value(), ico.pixmap(16,16), true);
                fastList.insert(i->getName().toUpper(), ww);
            }
        }
    }

    if (fastList.size() > 0)
    {
        for (QMultiMap<QString,contactWidgetFastBook*>::ConstIterator xx = fastList.begin(); xx != fastList.end(); ++xx)
        {
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
    close();
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
    for(int x = 0; x < ui->listWidget->count(); x++) {
        contactWidgetFastBook *w = static_cast<contactWidgetFastBook*>(ui->listWidget->itemWidget(ui->listWidget->item(x)));
        if (w->isChecked()) {
            QRecipientWidget *wi = new QRecipientWidget(w->getContact().getName(), w->getContact().getAccount(), w->getContact().getPhone(), w->getIcon());
            l.push_back(wi);
        }
    }
    emit addRecipients(l);
    close();

}

void multipleCheckDialog::on_button_all_clicked()
{
    for(int x = 0; x < ui->listWidget->count(); x++) {
        dynamic_cast<contactWidgetFastBook*>(ui->listWidget->itemWidget(ui->listWidget->item(x)))->setChecked(true);
    }
}
