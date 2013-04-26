/*
    Copyright (C) <2011>

    <enrico bacis> <enrico.bacis@gmail.com>
    <ivan vaccari> <grisson@jacksms.it>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "plugindialog.h"
#include "ui_plugindialog.h"
#include "pluginListItem.h"

pluginDialog::pluginDialog(QList<JackPluginInterface*> &_pluginsList,QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::pluginDialog),
    pluginsList(_pluginsList)

{
    m_ui->setupUi(this);

    QList<JackPluginInterface*>::const_iterator i=pluginsList.begin();
    QList<JackPluginInterface*>::const_iterator i_end=pluginsList.end();
    for(;i!=i_end;++i){
        m_ui->listPlugins->addItem(new PluginListItem(0, *i));
    }
    connect(m_ui->listPlugins, SIGNAL(itemSelectionChanged()), this, SLOT(setPluginInfo()));
}
void pluginDialog::setPluginInfo(){
    if(m_ui->listPlugins->selectedItems().count() > 0){
    PluginListItem* item = dynamic_cast<PluginListItem*>(m_ui->listPlugins->selectedItems().at(0));
    PluginInfo info = item->plugin->getPluginInfo();

    m_ui->label_name->setText(info.name);
    m_ui->label_type->setText(info.pluginTypeAsString());
    m_ui->label_version->setText(info.version);
    m_ui->label_info->setText(info.description);
    m_ui->label_author->setText(info.author);

    if(item->plugin->getPluginStatus() == Broken)
        m_ui->label_status->setText("<font color=\"red\">Errore</font>");
    else
        m_ui->label_status->setText("<font color=\"green\">OK</font>");

    connect(m_ui->button_config, SIGNAL(clicked()), item->plugin, SLOT(configure()));
    }

}
pluginDialog::~pluginDialog()
{
    delete m_ui;
}

void pluginDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
