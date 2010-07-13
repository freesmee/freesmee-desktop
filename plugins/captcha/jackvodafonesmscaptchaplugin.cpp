#include "jackvodafonesmscaptchaplugin.h"

JackVodafoneSMSCaptchaPlugin::JackVodafoneSMSCaptchaPlugin()
{
}

QString JackVodafoneSMSCaptchaPlugin::getServiceName(){
    return "Vodafone-SMS";
}

QString JackVodafoneSMSCaptchaPlugin::decodeCaptcha(QString imagePath){

   QProcess* ocr = new QProcess(this);

   //REMOVES BACKGROUND
   //imagemagick parameters:
   //inputfile
   //command
   //outputfile


   ocr->start("./plugins/" CONVERT_EXE
              " \"" + imagePath + "\"" +
              " -white-threshold 25000 " +
              " \""+ QDir::toNativeSeparators(QDir::tempPath()+ "/cap.bmp")+ "\"");

    ocr->waitForFinished(-1);
    ocr->close();



    //PERFORMS OCR READ
    ocr = new QProcess(this);
    ocr->start("./plugins/" TESSERACT_EXE " \"" + QDir::toNativeSeparators(QDir::tempPath()+ "/cap.bmp")+ "\" \""
              + QDir::toNativeSeparators(QDir::tempPath()+ "/cap") + "\"" + " -l vod");
    ocr->waitForFinished(-1);
    ocr->close();



    //tesseract automatically outputs to same dir + samefilename with txt extension
    QFile resultfile(QDir::tempPath()+"/cap.txt" );
    resultfile.open(QIODevice::ReadOnly);
    //read result from file
    QString result = resultfile.readAll();
    resultfile.close();

    //removes possible garbage
    result = result.remove("\n");
    result = result.remove(" ");

    //CLEANUP
    QFile::remove(QDir::tempPath()+"/cap.txt");
    QFile::remove(QDir::tempPath()+"/cap.bmp");

    return result;

}

PluginStatus JackVodafoneSMSCaptchaPlugin::getPluginStatus(){
    if(QFile::exists("./plugins/" TESSERACT_EXE) && QFile::exists("./plugins/" CONVERT_EXE))
        return Ready;
    else
        return Broken;
}

PluginInfo JackVodafoneSMSCaptchaPlugin::getPluginInfo(){
    PluginInfo info;
    info.name = "AntiCaptcha: Vodafone-SMS";
    info.author = "Master_T";
    info.pluginType = CaptchaDecoder;
    info.version = "0.3";
    info.description = "Un plugin per risolvere i captcha Vodafone-SMS";

    return info;
}

void JackVodafoneSMSCaptchaPlugin::configure(){
    QMessageBox::information(0, "Nessuna opzione da configurare per ora!", "Info");
}

Q_EXPORT_PLUGIN2(VodafoneSMSCaptchaPlugin, JackVodafoneSMSCaptchaPlugin)
