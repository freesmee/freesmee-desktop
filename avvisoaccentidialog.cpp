#include "avvisoaccentidialog.h"
#include "ui_avvisoaccentidialog.h"

avvisoaccentidialog::avvisoaccentidialog(QString & scelta,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::avvisoaccentidialog),
    sc(scelta)


{
    ui->setupUi(this);
}

avvisoaccentidialog::~avvisoaccentidialog()
{
    delete ui;
}

void avvisoaccentidialog::changeEvent(QEvent *e)
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

void avvisoaccentidialog::on_pushButton_2_clicked()
{
    sc="-1";
    close();
}

void avvisoaccentidialog::on_radioButton_clicked()
{
   sc="1";
}

void avvisoaccentidialog::on_radioButton_2_clicked()
{
    sc="2";
}

void avvisoaccentidialog::on_saveButton_clicked()
{
    close();
}
