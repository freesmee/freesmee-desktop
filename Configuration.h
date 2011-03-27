#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <QString>
#include <QByteArray>
#ifdef __APPLE__

QString getMacJackPath();
QString getJackPluginsPath();
#endif

#define JACKSMS_VERSION "3.0-a3"

#endif // CONFIGURATION_H
