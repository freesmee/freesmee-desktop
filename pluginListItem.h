#ifndef PLUGINITEM_H
#define PLUGINITEM_H
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QString>
#include "plugins/JackPluginInterfacer.h"

class PluginListItem : public QListWidgetItem {

  public:
    JackPluginInterface* plugin;

    PluginListItem(QListWidget* parent, JackPluginInterface* plg) : QListWidgetItem(parent)
    {
        plugin = plg;
        setText(plugin->getPluginInfo().name);
    }


};

#endif
