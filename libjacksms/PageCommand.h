#ifndef PAGECOMMAND_H
#define PAGECOMMAND_H
#include <QString>
namespace libJackSMS{
    namespace dataTypes{
        class pageCommand
        {
        private:
            QString cmd;
        public:
            pageCommand();
            pageCommand(const QString &_cmd);
            QString getCommand() const;
        };
   }
}
#endif // PAGECOMMAND_H
