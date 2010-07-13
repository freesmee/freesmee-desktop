#ifndef JACKSCRIBACOMPRESSORE_H
#define JACKSCRIBACOMPRESSORE_H



#include <QtCore>
#include <QtGui>

#include "JackScribaPluginInterfacer.h"

class JackScribaCompressore : public JackScribaPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(JackScribaPluginInterface)


public:
    JackScribaCompressore();

    void processText(QString&);
    PluginStatus getPluginStatus();
    PluginInfo getPluginInfo();
public slots:
    void configure();
};

#endif // JACKSCRIBACOMPRESSORE_H
