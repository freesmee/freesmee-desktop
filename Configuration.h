#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <QString>
#include <QByteArray>
#ifdef __APPLE__

QString getMacJackPath();
QString getJackPluginsPath();
#endif



#define JACKSMS_VERSION "2.0-b3"

QByteArray hardcodedServicesFile();

#endif // CONFIGURATION_H
