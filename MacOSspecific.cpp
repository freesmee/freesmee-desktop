#include <QString>
#include "CoreFoundation/CFBundle.h"


QString getMacJackPath(){
    QString macPathq;

    CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
    const char *pathPtr = CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());

    macPathq = QString::fromStdString(pathPtr);

    CFRelease(appUrlRef);
    CFRelease(macPath);
    return macPathq + "/Contents/Resources/JackSms";
}

QString getJackPluginsPath(){
    QString macPathq;

    CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
    const char *pathPtr = CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());

    macPathq = QString::fromStdString(pathPtr);

    CFRelease(appUrlRef);
    CFRelease(macPath);

    return macPathq + "/Contents/PlugIns";
}
