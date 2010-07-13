#ifndef AVVISOACCENTIDIALOG_H
#define AVVISOACCENTIDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class avvisoaccentidialog;
}

class avvisoaccentidialog : public QDialog {
    Q_OBJECT
public:
    avvisoaccentidialog(QString & scelta,QWidget *parent = 0);
    ~avvisoaccentidialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::avvisoaccentidialog *ui;
    QString &sc;
private slots:
    void on_saveButton_clicked();
    void on_radioButton_2_clicked();
    void on_radioButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // AVVISOACCENTIDIALOG_H
