#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#ifdef __APPLE__
#include <QString>
QString getMacJackPath();
QString getJackPluginsPath();
#endif



#define JACKSMS_VERSION "2.0-b2"

#endif // CONFIGURATION_H
