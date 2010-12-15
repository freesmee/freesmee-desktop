#ifndef QRECIPIENTWIDGET_H
#define QRECIPIENTWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QString>
#include "qlabelresult.h"
#include <QListWidgetItem>
#include "libjacksms/libJackSMS.h"
class QRecipientWidget : public QWidget
{
Q_OBJECT
private:
    QHBoxLayout *hLayout;
    QLabel *name;
    QFrame *f1;
    libJackSMS::dataTypes::phoneNumber number;
    QLabel *iconStatus;
    QLabel *iconService;
    QString accountId;
    QString contactName;
    QLabelResult *removeLabel;
    QListWidgetItem *pIt;
    QLabel *iconLabel;
public:
    void setStatusFailed(QString message);
    void setStatusWorking();
    void setStatusSuccess(QString message);
    QRecipientWidget(const QString & contactName,const QString & accId,const libJackSMS::dataTypes::phoneNumber &number,QPixmap icon,QWidget *parent = 0);
    libJackSMS::dataTypes::phoneNumber getPhone() const;
    QString getAccountId() const;
    void setParentItem(QListWidgetItem * it);


signals:
    void removed(QListWidgetItem*);
private slots:
    void remove();

};

#endif // QRECIPIENTWIDGET_H
