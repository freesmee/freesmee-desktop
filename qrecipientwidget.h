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
    QLabel *iconStatus;
    QLabel *iconService;
    QString contactName;
    QString accountId;
    libJackSMS::dataTypes::phoneNumber number;
    QLabelResult *removeLabel;
    QListWidgetItem *pIt;
    QLabel *iconLabel;
    QPixmap original;
    QString nameStr;
public:
    void setStatusFailed(QString message);
    void setStatusWorking();
    void setStatusSuccess(QString message);
    QRecipientWidget(const QString & contactName,const QString & accId,const libJackSMS::dataTypes::phoneNumber &number,QPixmap icon,QWidget *parent = 0);
    libJackSMS::dataTypes::phoneNumber getPhone() const;
    QString getAccountId() const;
    void setParentItem(QListWidgetItem * it);
    void setIcon(QPixmap Icon);
    void restoreOriginalIcon();
    QString getName() const;
    void clearStatus();
signals:
    void removed(QListWidgetItem*);
private slots:
    void remove();

};

#endif // QRECIPIENTWIDGET_H
