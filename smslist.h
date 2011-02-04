#ifndef SMSLIST_H
#define SMSLIST_H

#include <QListWidget>
#include <QKeyEvent>
#include <QClipboard>
#include <QApplication>
#include "smswidget.h"

class SmsList : public QListWidget
{
    Q_OBJECT
public:
    explicit SmsList(QWidget *parent = 0);
    void hideCaricaAltri(bool hide);
    void addItem(QListWidgetItem *item);
    void clear();

private:
    void keyPressEvent(QKeyEvent* e);

signals:
    void smsListCanc();

public slots:

};

#endif // SMSLIST_H
