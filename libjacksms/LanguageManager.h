#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H
#include <QMap>
#include <QList>
#include <QString>

/*using singleton design pattern*/
namespace libJackSMS{
    namespace dataTypes{
        class Language
        {
        private:
            QMap<QString,QString> coppie;
            QString langName;
        public:
            Language(QString _lname);
            void setString(const QString _id, const QString _string);
            QString getString(const QString _id)const;
            QString getLangName() const;
        };

    }
}
namespace libJackSMS{

    class LanguageManager
    {
    private:
        LanguageManager();
        static LanguageManager* istance;
        QList<dataTypes::Language> languages;
        QList<dataTypes::Language>::const_iterator currentLanguage;
        QList<dataTypes::Language>::const_iterator lanIter;
        bool isSet;
        int lan_iterator_counter;

    public:

        static LanguageManager* getIstance();
        QString getString(const QString &_id);
        bool setLanguage(QString _name);
        bool nextLanguage();
        QString currentLang() const;

    };
}
#endif // LANGUAGEMANAGER_H
