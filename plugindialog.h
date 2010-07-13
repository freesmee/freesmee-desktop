#ifndef PLUGINDIALOG_H
#define PLUGINDIALOG_H

#include <QtGui/QDialog>
#include "plugins/JackPluginInterfacer.h"
namespace Ui {
    class pluginDialog;
}

class pluginDialog : public QDialog {
    Q_OBJECT
public:
    pluginDialog(QList<JackPluginInterface*> &_pluginsList,QWidget *parent = 0);
    ~pluginDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::pluginDialog *m_ui;
    QList<JackPluginInterface*> &pluginsList;

private slots:
    void setPluginInfo();
};

#endif // PLUGINDIALOG_H
